#include "esp.h"
#include "../utilities/memory.h"
#include "../../a2x/offsets.hpp"
#include "../interfaces/interfaces.h"
#include "../../CheatData.h"
#include "../menu/config.h"

void EspDrawManager::DrawFrame(ImDrawList* drawlist){

	for (auto& i : manydogs){
		ImVec2 screen,top_screen;
		

		Vector world_origin = i.pos;
		Vector world_origin_top = world_origin;
		world_origin_top.z += i.size.y;

		WorldToScreen(world_origin, &screen);
		WorldToScreen(world_origin_top, &top_screen);

		//drawlist->AddLine(screen, top_screen, IM_COL32_WHITE);

		int box_h = screen.y - top_screen.y;
		ImVec2 min,max;
		min = ImVec2(top_screen.x - box_h / 4,top_screen.y);
		max = ImVec2(top_screen.x + box_h / 4, screen.y);
		//drawlist->AddRect(min,max, IM_COL32_WHITE);

		if (*g_ConfigManager->GetBool("esp_box")){
			drawlist->AddRect(ImVec2(i.box.x, i.box.y), ImVec2(i.box.z, i.box.w), i.is_visible ? IM_COL32(255, 0, 0, 255) : IM_COL32_WHITE);
		}
		


		/*ImVec2 eyepos_screen;
		WorldToScreen(i.eyepos,&eyepos_screen);
		drawlist->AddLine(ImVec2(0,0), eyepos_screen, IM_COL32_WHITE);*/

	}


	//drawlist->AddLine(ImVec2(0, 0), ImVec2(1920,1080), IM_COL32(0, 0, 0, 255));


}





//call in fsn
void EspDrawManager::MakeFrame(){
	manydogs.clear();

	for (int i = 2; i < g_interfaces->GameResourceService->pGameEntitySystem->GetHighestEntityIndex(); i++)
	{
		auto shit = g_interfaces->GameResourceService->pGameEntitySystem->Get(i);
		if (shit == nullptr)
			continue;


		if (shit->IsBasePlayerController())
		{
			auto shit_controller = reinterpret_cast<CCSPlayerController*>(shit);
			C_CSPlayerPawn* pPlayerPawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(shit_controller->GetPawnHandle());
			//LOG(INFO) << shit_controller->GetPawnHealth() << " - " << pPlayerPawn->GetHealth() << " - " << shit_controller->GetHealth() << " - " << shit_controller->IsPawnAlive();

			//std::cout << pPlayerPawn->GetHealth() << "\n";

			if (shit_controller->IsPawnAlive() && pPlayerPawn->GetHealth()) {

				auto& esp = manydogs.emplace_back(shit_controller->GetPawnOrigin(), pPlayerPawn->GetEyePosition(), ImVec2(50, 70));


				EspItemElement name;
				name.pos = EspItemElement::TOP;
				name.type = EspItemElement::TEXT;
				name.text = shit_controller->GetPlayerName();

				esp.Elements.emplace_back(name);


				ImVec4 box;

				GetEntityBoundingBox(pPlayerPawn,&box);

				esp.eyepos = pPlayerPawn->GetEyePosition();
				esp.box = box;
				if(g_CheatData->LocalPawn)
				esp.is_visible = pPlayerPawn->Visible(g_CheatData->LocalPawn);
			}
			



		}
		



	} 
	





}

bool EspDrawManager::WorldToScreen(const Vector& vecOrigin, ImVec2* pvecScreen)
{
	const float flWidth = matrix[3][0] * vecOrigin.x + matrix[3][1] * vecOrigin.y + matrix[3][2] * vecOrigin.z + matrix[3][3];

	// check is point can't fit on screen, because it's behind us
	if (flWidth < 0.001f)
		return false;

	// compute the scene coordinates of a point in 3D
	const float flInverse = 1.0f / flWidth;
	pvecScreen->x = (matrix[0][0] * vecOrigin.x + matrix[0][1] * vecOrigin.y + matrix[0][2] * vecOrigin.z + matrix[0][3]) * flInverse;
	pvecScreen->y = (matrix[1][0] * vecOrigin.x + matrix[1][1] * vecOrigin.y + matrix[1][2] * vecOrigin.z + matrix[1][3]) * flInverse;

	// screen transform
	// get the screen position in pixels of given point
	const ImVec2 vecDisplaySize = ImGui::GetIO().DisplaySize;
	pvecScreen->x = (vecDisplaySize.x * 0.5f) + (pvecScreen->x * vecDisplaySize.x) * 0.5f;
	pvecScreen->y = (vecDisplaySize.y * 0.5f) - (pvecScreen->y * vecDisplaySize.y) * 0.5f;
	return true;
}

bool EspDrawManager::GetEntityBoundingBox(void* pEntity, ImVec4* pVecOut)
{
	auto player = (C_CSPlayerPawn*)pEntity;
	CCollisionProperty* pCollision = player->GetCollision();
	if (pCollision == nullptr)
		return false;

	CGameSceneNode* pGameSceneNode = player->GetGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	CTransform nodeToWorldTransform = pGameSceneNode->GetNodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);

	const Vector vecMins = pCollision->m_vecMins();
	const Vector vecMaxs = pCollision->m_vecMaxs();

	pVecOut->x = pVecOut->y = 3.402823466e+38F;
	pVecOut->z = pVecOut->w = -3.402823466e+38F;

	for (int i = 0; i < 8; ++i)
	{
		const Vector vecPoint{
			i & 1 ? vecMaxs.x : vecMins.x,
			i & 2 ? vecMaxs.y : vecMins.y,
			i & 4 ? vecMaxs.z : vecMins.z
		};
		ImVec2 vecScreen;
		if (!WorldToScreen(vecPoint.Transform(matTransform), &vecScreen))
			return false;

		pVecOut->x = MATH::Min(pVecOut->x, vecScreen.x);
		pVecOut->y = MATH::Min(pVecOut->y, vecScreen.y);
		pVecOut->z = MATH::Max(pVecOut->z, vecScreen.x);
		pVecOut->w = MATH::Max(pVecOut->w, vecScreen.y);
	}

	return true;
}
