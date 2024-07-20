#include "esp.h"
#include "../utilities/memory.h"
#include "../../a2x/offsets.hpp"
#include "../interfaces/interfaces.h"

void EspDrawManager::DrawFrame(ImDrawList* drawlist){

	for (auto& i : manydogs){
		ImVec2 screen;
		WorldToScreen(i.pos,&screen);

		drawlist->AddLine(ImVec2(0,0),screen, IM_COL32(0, 0, 0, 255));




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
			LOG(INFO) << shit_controller->GetPawnHealth() << " - " << pPlayerPawn->GetHealth() << " - " << shit_controller->GetHealth() << " - " << shit_controller->IsPawnAlive();

			std::cout << pPlayerPawn->GetHealth() << "\n";

			//if(shit_controller->GetPawnHealth())
			manydogs.emplace_back(shit_controller->GetPawnOrigin(), ImVec2());



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
