#include "pch.h"
#include "EntityCopier.h"
#include "ecs/component/Children.h"


// Constructors

EntityCopier::EntityCopier(entt::registry& aDstReg, const entt::registry& aSrcReg)
	: myDstReg(aDstReg)
	, mySrcReg(aSrcReg)
{
}

EntityCopier::EntityCopier(entt::registry& aDstAndSrcReg)
	: EntityCopier(aDstAndSrcReg, aDstAndSrcReg)
{
}


// Copy methods

entt::entity EntityCopier::ShallowCopy(entt::entity aSrcEntity)
{
	if (!mySrcReg.valid(aSrcEntity))
		return entt::null;

	entt::entity dstEntity = myDstReg.create();

	for (auto [srcName, srcStorage] : mySrcReg.storage())
	{
		if (!srcStorage.contains(aSrcEntity))
			continue;

		if (srcStorage.type() == entt::type_id<Children>())
			continue;

		auto dstStorage = myDstReg.storage(srcName);
		if (dstStorage != myDstReg.storage().cend())
			dstStorage->second.emplace(dstEntity, srcStorage.get(aSrcEntity));
	}

	return dstEntity;
}

entt::entity EntityCopier::DeepCopy(entt::entity aSrcEntity)
{
	if (!mySrcReg.valid(aSrcEntity))
		return entt::null;

	entt::entity dstEntity = myDstReg.create();

	for (auto [srcName, srcStorage] : mySrcReg.storage())
	{
		if (!srcStorage.contains(aSrcEntity))
			continue;

		if (srcStorage.type() != entt::type_id<Children>())
		{
			auto dstStorage = myDstReg.storage(srcName);
			if (dstStorage != myDstReg.storage().cend())
				dstStorage->second.emplace(dstEntity, srcStorage.get(aSrcEntity));
			continue;
		}

		const Children& srcChildren = mySrcReg.get<Children>(aSrcEntity);
		Children& dstChildren = myDstReg.emplace<Children>(dstEntity);
		dstChildren.resize(srcChildren.size());
		rn::transform(srcChildren, dstChildren.begin(), [this](entt::entity srcChild) { return DeepCopy(srcChild); });
	}

	return dstEntity;
}
