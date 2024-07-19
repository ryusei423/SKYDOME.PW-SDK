#include"schema.h"
#include "../interfaces/interfaces.h"

bool SchemaManager::init(const char* module)
{

	CSchemaSystemTypeScope* pTypeScope = g_interfaces->SchemaSystem->FindTypeScopeForModule(module);
	if (pTypeScope == nullptr)
		return false;

	const int nTableSize = pTypeScope->hashClasses.Count();
	LOG(INFO) << XorStr("found \"") << nTableSize << XorStr("\" schema classes in module");

	// allocate memory for elements
	UtlTSHashHandle_t* pElements = new UtlTSHashHandle_t[nTableSize + 1U];
	const auto nElements = pTypeScope->hashClasses.GetElements(0, nTableSize, pElements);

	for (int i = 0; i < nElements; i++)
	{
		const UtlTSHashHandle_t hElement = pElements[i];

		if (hElement == 0)
			continue;

		CSchemaClassBinding* pClassBinding = pTypeScope->hashClasses[hElement];
		if (pClassBinding == nullptr)
			continue;

		SchemaClassInfoData_t* pDeclaredClassInfo;
		pTypeScope->FindDeclaredClass(&pDeclaredClassInfo, pClassBinding->szBinaryName);

		if (pDeclaredClassInfo == nullptr)
			continue;

		if (pDeclaredClassInfo->nFieldSize == 0)
			continue;

		CRT::String_t<MAX_PATH> szClassBuffer(XorStr("class %s\n"), pDeclaredClassInfo->szName);

		for (auto j = 0; j < pDeclaredClassInfo->nFieldSize; j++)
		{
			SchemaClassFieldData_t* pFields = pDeclaredClassInfo->pFields;
			CRT::String_t<MAX_PATH> szFieldClassBuffer(XorStr("%s->%s"), pClassBinding->szBinaryName, pFields[j].szName);
			// store field info
			//vecSchemaData.emplace_back(FNV1A::Hash(szFieldClassBuffer.Data()), pFields[j].nSingleInheritanceOffset);

			offsets[szFieldClassBuffer.Data()] = pFields[j].nSingleInheritanceOffset;

			CRT::String_t<MAX_PATH> szFieldBuffer(XorStr("    %s %s = 0x%X\n"), pFields[j].pSchemaType->szName, pFields[j].szName, pFields[j].nSingleInheritanceOffset);

		}
#ifdef _DEBUG
		//LOG(INFO) << XorStr("dumped \"") << pDeclaredClassInfo->szName << XorStr("\" (total: ") << pDeclaredClassInfo->nFieldSize << XorStr(" fields)");
#endif
	}



	delete[] pElements;
	return true;
}

std::uint32_t SchemaManager::GetOffset(const std::string name)
{
	return offsets[name];
}
