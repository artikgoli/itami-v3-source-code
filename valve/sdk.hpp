#pragma once
namespace sdk
{
    class C_BaseEntity
    {
    public:
        static constexpr std::uint32_t m_CBodyComponent = 0x38;
        static constexpr std::uint32_t m_NetworkTransmitComponent = 0x40;
        static constexpr std::uint32_t m_nLastThinkTick = 0x330;
        static constexpr std::uint32_t m_pGameSceneNode = 0x338;
        static constexpr std::uint32_t m_pRenderComponent = 0x340;
        static constexpr std::uint32_t m_pCollision = 0x348;
        static constexpr std::uint32_t m_iMaxHealth = 0x350;
        static constexpr std::uint32_t m_iHealth = 0x354;
        static constexpr std::uint32_t m_flDamageAccumulator = 0x358;
        static constexpr std::uint32_t m_lifeState = 0x35c;
        static constexpr std::uint32_t m_bTakesDamage = 0x35d;
        static constexpr std::uint32_t m_nTakeDamageFlags = 0x360;
        static constexpr std::uint32_t m_nPlatformType = 0x368;
        static constexpr std::uint32_t m_ubInterpolationFrame = 0x369;
        static constexpr std::uint32_t m_hSceneObjectController = 0x36c;
        static constexpr std::uint32_t m_nNoInterpolationTick = 0x370;
        static constexpr std::uint32_t m_nVisibilityNoInterpolationTick = 0x374;
        static constexpr std::uint32_t m_flProxyRandomValue = 0x378;
        static constexpr std::uint32_t m_iEFlags = 0x37c;
        static constexpr std::uint32_t m_nWaterType = 0x380;
        static constexpr std::uint32_t m_bInterpolateEvenWithNoModel = 0x381;
        static constexpr std::uint32_t m_bPredictionEligible = 0x382;
        static constexpr std::uint32_t m_bApplyLayerMatchIDToModel = 0x383;
        static constexpr std::uint32_t m_tokLayerMatchID = 0x384;
        static constexpr std::uint32_t m_nSubclassID = 0x388;
        static constexpr std::uint32_t m_nSimulationTick = 0x398;
        static constexpr std::uint32_t m_iCurrentThinkContext = 0x39c;
        static constexpr std::uint32_t m_aThinkFunctions = 0x3a0;
        static constexpr std::uint32_t m_bDisabledContextThinks = 0x3b8;
        static constexpr std::uint32_t m_flAnimTime = 0x3bc;
        static constexpr std::uint32_t m_flSimulationTime = 0x3c0;
        static constexpr std::uint32_t m_nSceneObjectOverrideFlags = 0x3c4;
        static constexpr std::uint32_t m_bHasSuccessfullyInterpolated = 0x3c5;
        static constexpr std::uint32_t m_bHasAddedVarsToInterpolation = 0x3c6;
        static constexpr std::uint32_t m_bRenderEvenWhenNotSuccessfullyInterpolated = 0x3c7;
        static constexpr std::uint32_t m_nInterpolationLatchDirtyFlags = 0x3c8;
        static constexpr std::uint32_t m_ListEntry = 0x3d0;
        static constexpr std::uint32_t m_flCreateTime = 0x3e8;
        static constexpr std::uint32_t m_flSpeed = 0x3ec;
        static constexpr std::uint32_t m_EntClientFlags = 0x3f0;
        static constexpr std::uint32_t m_bClientSideRagdoll = 0x3f2;
        static constexpr std::uint32_t m_iTeamNum = 0x3f3;
        static constexpr std::uint32_t m_spawnflags = 0x3f4;
        static constexpr std::uint32_t m_nNextThinkTick = 0x3f8;
        static constexpr std::uint32_t m_fFlags = 0x400;
        static constexpr std::uint32_t m_vecAbsVelocity = 0x404;
        static constexpr std::uint32_t m_vecServerVelocity = 0x410;
        static constexpr std::uint32_t m_vecVelocity = 0x438;
        static constexpr std::uint32_t m_vecBaseVelocity = 0x518;
        static constexpr std::uint32_t m_hEffectEntity = 0x524;
        static constexpr std::uint32_t m_hOwnerEntity = 0x528;
        static constexpr std::uint32_t m_MoveCollide = 0x52c;
        static constexpr std::uint32_t m_MoveType = 0x52d;
        static constexpr std::uint32_t m_nActualMoveType = 0x52e;
        static constexpr std::uint32_t m_flWaterLevel = 0x530;
        static constexpr std::uint32_t m_fEffects = 0x534;
        static constexpr std::uint32_t m_hGroundEntity = 0x538;
        static constexpr std::uint32_t m_nGroundBodyIndex = 0x53c;
        static constexpr std::uint32_t m_flFriction = 0x540;
        static constexpr std::uint32_t m_flElasticity = 0x544;
        static constexpr std::uint32_t m_flGravityScale = 0x548;
        static constexpr std::uint32_t m_flTimeScale = 0x54c;
        static constexpr std::uint32_t m_bAnimatedEveryTick = 0x550;
        static constexpr std::uint32_t m_bGravityDisabled = 0x551;
        static constexpr std::uint32_t m_flNavIgnoreUntilTime = 0x554;
        static constexpr std::uint32_t m_hThink = 0x558;
        static constexpr std::uint32_t m_fBBoxVisFlags = 0x568;
        static constexpr std::uint32_t m_flActualGravityScale = 0x56c;
        static constexpr std::uint32_t m_bGravityActuallyDisabled = 0x570;
        static constexpr std::uint32_t m_bPredictable = 0x571;
        static constexpr std::uint32_t m_bRenderWithViewModels = 0x572;
        static constexpr std::uint32_t m_nFirstPredictableCommand = 0x574;
        static constexpr std::uint32_t m_nLastPredictableCommand = 0x578;
        static constexpr std::uint32_t m_hOldMoveParent = 0x57c;
        static constexpr std::uint32_t m_Particles = 0x580;
        static constexpr std::uint32_t m_vecAngVelocity = 0x5b0;
        static constexpr std::uint32_t m_DataChangeEventRef = 0x5bc;
        static constexpr std::uint32_t m_dependencies = 0x5c0;
        static constexpr std::uint32_t m_nCreationTick = 0x5d8;
        static constexpr std::uint32_t m_bAnimTimeChanged = 0x5e9;
        static constexpr std::uint32_t m_bSimulationTimeChanged = 0x5ea;
        static constexpr std::uint32_t m_sUniqueHammerID = 0x5f8;
        static constexpr std::uint32_t m_nBloodType = 0x600;
    };

    class C_PointCamera
    {
    public:
        static constexpr std::uint32_t m_FOV = 0x608;
        static constexpr std::uint32_t m_Resolution = 0x60c;
        static constexpr std::uint32_t m_bFogEnable = 0x610;
        static constexpr std::uint32_t m_FogColor = 0x611;
        static constexpr std::uint32_t m_flFogStart = 0x618;
        static constexpr std::uint32_t m_flFogEnd = 0x61c;
        static constexpr std::uint32_t m_flFogMaxDensity = 0x620;
        static constexpr std::uint32_t m_bActive = 0x624;
        static constexpr std::uint32_t m_bUseScreenAspectRatio = 0x625;
        static constexpr std::uint32_t m_flAspectRatio = 0x628;
        static constexpr std::uint32_t m_bNoSky = 0x62c;
        static constexpr std::uint32_t m_fBrightness = 0x630;
        static constexpr std::uint32_t m_flZFar = 0x634;
        static constexpr std::uint32_t m_flZNear = 0x638;
        static constexpr std::uint32_t m_bCanHLTVUse = 0x63c;
        static constexpr std::uint32_t m_bAlignWithParent = 0x63d;
        static constexpr std::uint32_t m_bDofEnabled = 0x63e;
        static constexpr std::uint32_t m_flDofNearBlurry = 0x640;
        static constexpr std::uint32_t m_flDofNearCrisp = 0x644;
        static constexpr std::uint32_t m_flDofFarCrisp = 0x648;
        static constexpr std::uint32_t m_flDofFarBlurry = 0x64c;
        static constexpr std::uint32_t m_flDofTiltToGround = 0x650;
        static constexpr std::uint32_t m_TargetFOV = 0x654;
        static constexpr std::uint32_t m_DegreesPerSecond = 0x658;
        static constexpr std::uint32_t m_bIsOn = 0x65c;
        static constexpr std::uint32_t m_pNext = 0x660;
    };

    class C_CommandContext
    {
    public:
        static constexpr std::uint32_t needsprocessing = 0x0;
        static constexpr std::uint32_t command_number = 0xa0;
    };

    class C_ColorCorrection
    {
    public:
        static constexpr std::uint32_t m_vecOrigin = 0x608;
        static constexpr std::uint32_t m_MinFalloff = 0x614;
        static constexpr std::uint32_t m_MaxFalloff = 0x618;
        static constexpr std::uint32_t m_flFadeInDuration = 0x61c;
        static constexpr std::uint32_t m_flFadeOutDuration = 0x620;
        static constexpr std::uint32_t m_flMaxWeight = 0x624;
        static constexpr std::uint32_t m_flCurWeight = 0x628;
        static constexpr std::uint32_t m_netlookupFilename = 0x62c;
        static constexpr std::uint32_t m_bEnabled = 0x82c;
        static constexpr std::uint32_t m_bMaster = 0x82d;
        static constexpr std::uint32_t m_bClientSide = 0x82e;
        static constexpr std::uint32_t m_bExclusive = 0x82f;
        static constexpr std::uint32_t m_bEnabledOnClient = 0x830;
        static constexpr std::uint32_t m_flCurWeightOnClient = 0x834;
        static constexpr std::uint32_t m_bFadingIn = 0x838;
        static constexpr std::uint32_t m_flFadeStartWeight = 0x83c;
        static constexpr std::uint32_t m_flFadeStartTime = 0x840;
        static constexpr std::uint32_t m_flFadeDuration = 0x844;
    };

    class C_TonemapController2
    {
    public:
        static constexpr std::uint32_t m_flAutoExposureMin = 0x608;
        static constexpr std::uint32_t m_flAutoExposureMax = 0x60c;
        static constexpr std::uint32_t m_flExposureAdaptationSpeedUp = 0x610;
        static constexpr std::uint32_t m_flExposureAdaptationSpeedDown = 0x614;
        static constexpr std::uint32_t m_flTonemapEVSmoothingRange = 0x618;
    };

    class C_PostProcessingVolume
    {
    public:
        static constexpr std::uint32_t m_hPostSettings = 0xf68;
        static constexpr std::uint32_t m_flFadeDuration = 0xf70;
        static constexpr std::uint32_t m_flMinLogExposure = 0xf74;
        static constexpr std::uint32_t m_flMaxLogExposure = 0xf78;
        static constexpr std::uint32_t m_flMinExposure = 0xf7c;
        static constexpr std::uint32_t m_flMaxExposure = 0xf80;
        static constexpr std::uint32_t m_flExposureCompensation = 0xf84;
        static constexpr std::uint32_t m_flExposureFadeSpeedUp = 0xf88;
        static constexpr std::uint32_t m_flExposureFadeSpeedDown = 0xf8c;
        static constexpr std::uint32_t m_flTonemapEVSmoothingRange = 0xf90;
        static constexpr std::uint32_t m_bMaster = 0xf94;
        static constexpr std::uint32_t m_bExposureControl = 0xf95;
    };

    class C_FogController
    {
    public:
        static constexpr std::uint32_t m_fog = 0x608;
        static constexpr std::uint32_t m_bUseAngles = 0x670;
        static constexpr std::uint32_t m_iChangedVariables = 0x674;
    };

    class C_BasePlayerWeapon
    {
    public:
        static constexpr std::uint32_t m_nNextPrimaryAttackTick = 0x18c0;
        static constexpr std::uint32_t m_flNextPrimaryAttackTickRatio = 0x18c4;
        static constexpr std::uint32_t m_nNextSecondaryAttackTick = 0x18c8;
        static constexpr std::uint32_t m_flNextSecondaryAttackTickRatio = 0x18cc;
        static constexpr std::uint32_t m_iClip1 = 0x18d0;
        static constexpr std::uint32_t m_iClip2 = 0x18d4;
        static constexpr std::uint32_t m_pReserveAmmo = 0x18d8;
    };

    class C_BaseModelEntity
    {
    public:
        static constexpr std::uint32_t m_CRenderComponent = 0xaf0;
        static constexpr std::uint32_t m_CHitboxComponent = 0xaf8;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed0 = 0xb10;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed1 = 0xb14;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed2 = 0xb18;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed3 = 0xb1c;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed4 = 0xb20;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed0_PartIndex = 0xb24;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed1_PartIndex = 0xb28;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed2_PartIndex = 0xb2c;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed3_PartIndex = 0xb30;
        static constexpr std::uint32_t m_nDestructiblePartInitialStateDestructed4_PartIndex = 0xb34;
        static constexpr std::uint32_t m_pDestructiblePartsSystemComponent = 0xb38;
        static constexpr std::uint32_t m_bInitModelEffects = 0xb58;
        static constexpr std::uint32_t m_bDoingModelEffects = 0xb59;
        static constexpr std::uint32_t m_bIsStaticProp = 0xb5a;
        static constexpr std::uint32_t m_iOldHealth = 0xb5c;
        static constexpr std::uint32_t m_nRenderMode = 0xb60;
        static constexpr std::uint32_t m_nRenderFX = 0xb61;
        static constexpr std::uint32_t m_bAllowFadeInView = 0xb62;
        static constexpr std::uint32_t m_clrRender = 0xb80;
        static constexpr std::uint32_t m_vecRenderAttributes = 0xb88;
        static constexpr std::uint32_t m_bRenderToCubemaps = 0xc08;
        static constexpr std::uint32_t m_bNoInterpolate = 0xc09;
        static constexpr std::uint32_t m_Collision = 0xc10;
        static constexpr std::uint32_t m_Glow = 0xcc0;
        static constexpr std::uint32_t m_flGlowBackfaceMult = 0xd18;
        static constexpr std::uint32_t m_fadeMinDist = 0xd1c;
        static constexpr std::uint32_t m_fadeMaxDist = 0xd20;
        static constexpr std::uint32_t m_flFadeScale = 0xd24;
        static constexpr std::uint32_t m_flShadowStrength = 0xd28;
        static constexpr std::uint32_t m_nObjectCulling = 0xd2c;
        static constexpr std::uint32_t m_nRequiredDecalRtEncoding = 0xd2d;
        static constexpr std::uint32_t m_vecViewOffset = 0xd58;
        static constexpr std::uint32_t m_pClientAlphaProperty = 0xe38;
        static constexpr std::uint32_t m_ClientOverrideTint = 0xe40;
        static constexpr std::uint32_t m_bUseClientOverrideTint = 0xe44;
        static constexpr std::uint32_t m_bvDisabledHitGroups = 0xe80;
    };

    class C_BaseEntityAPI
    {
    public:
    };

    class C_EntityFlame
    {
    public:
        static constexpr std::uint32_t m_hEntAttached = 0x608;
        static constexpr std::uint32_t m_hOldAttached = 0x630;
        static constexpr std::uint32_t m_bCheapEffect = 0x634;
    };

    class C_RopeKeyframe
    {
    public:
        static constexpr std::uint32_t m_LinksTouchingSomething = 0xe90;
        static constexpr std::uint32_t m_nLinksTouchingSomething = 0xe94;
        static constexpr std::uint32_t m_bApplyWind = 0xe98;
        static constexpr std::uint32_t m_fPrevLockedPoints = 0xe9c;
        static constexpr std::uint32_t m_iForcePointMoveCounter = 0xea0;
        static constexpr std::uint32_t m_bPrevEndPointPos = 0xea4;
        static constexpr std::uint32_t m_vPrevEndPointPos = 0xea8;
        static constexpr std::uint32_t m_flCurScroll = 0xec0;
        static constexpr std::uint32_t m_flScrollSpeed = 0xec4;
        static constexpr std::uint32_t m_RopeFlags = 0xec8;
        static constexpr std::uint32_t m_iRopeMaterialModelIndex = 0xed0;
        static constexpr std::uint32_t m_nSegments = 0x1148;
        static constexpr std::uint32_t m_hStartPoint = 0x114c;
        static constexpr std::uint32_t m_hEndPoint = 0x1150;
        static constexpr std::uint32_t m_iStartAttachment = 0x1154;
        static constexpr std::uint32_t m_iEndAttachment = 0x1155;
        static constexpr std::uint32_t m_Subdiv = 0x1156;
        static constexpr std::uint32_t m_RopeLength = 0x1158;
        static constexpr std::uint32_t m_Slack = 0x115a;
        static constexpr std::uint32_t m_TextureScale = 0x115c;
        static constexpr std::uint32_t m_fLockedPoints = 0x1160;
        static constexpr std::uint32_t m_nChangeCount = 0x1161;
        static constexpr std::uint32_t m_Width = 0x1164;
        static constexpr std::uint32_t m_PhysicsDelegate = 0x1168;
        static constexpr std::uint32_t m_hMaterial = 0x1178;
        static constexpr std::uint32_t m_TextureHeight = 0x1180;
        static constexpr std::uint32_t m_vecImpulse = 0x1184;
        static constexpr std::uint32_t m_vecPreviousImpulse = 0x1190;
        static constexpr std::uint32_t m_flCurrentGustTimer = 0x119c;
        static constexpr std::uint32_t m_flCurrentGustLifetime = 0x11a0;
        static constexpr std::uint32_t m_flTimeToNextGust = 0x11a4;
        static constexpr std::uint32_t m_vWindDir = 0x11a8;
        static constexpr std::uint32_t m_vColorMod = 0x11b4;
        static constexpr std::uint32_t m_vCachedEndPointAttachmentPos = 0x11c0;
        static constexpr std::uint32_t m_vCachedEndPointAttachmentAngle = 0x11d8;
        static constexpr std::uint32_t m_bConstrainBetweenEndpoints = 0x11f0;
        static constexpr std::uint32_t m_bEndPointAttachmentPositionsDirty = 0x0;
        static constexpr std::uint32_t m_bEndPointAttachmentAnglesDirty = 0x0;
        static constexpr std::uint32_t m_bNewDataThisFrame = 0x0;
        static constexpr std::uint32_t m_bPhysicsInitted = 0x0;
    };

    class C_TintController
    {
    public:
    };

    class C_SkyCamera
    {
    public:
        static constexpr std::uint32_t m_skyboxData = 0x608;
        static constexpr std::uint32_t m_skyboxSlotToken = 0x698;
        static constexpr std::uint32_t m_bUseAngles = 0x69c;
        static constexpr std::uint32_t m_pNext = 0x6a0;
    };

    class C_EnvDetailController
    {
    public:
        static constexpr std::uint32_t m_flFadeStartDist = 0x608;
        static constexpr std::uint32_t m_flFadeEndDist = 0x60c;
    };

    class C_EnvWindShared
    {
    public:
        static constexpr std::uint32_t m_flStartTime = 0x8;
        static constexpr std::uint32_t m_iWindSeed = 0xc;
        static constexpr std::uint32_t m_iMinWind = 0x10;
        static constexpr std::uint32_t m_iMaxWind = 0x12;
        static constexpr std::uint32_t m_windRadius = 0x14;
        static constexpr std::uint32_t m_iMinGust = 0x18;
        static constexpr std::uint32_t m_iMaxGust = 0x1a;
        static constexpr std::uint32_t m_flMinGustDelay = 0x1c;
        static constexpr std::uint32_t m_flMaxGustDelay = 0x20;
        static constexpr std::uint32_t m_flGustDuration = 0x24;
        static constexpr std::uint32_t m_iGustDirChange = 0x28;
        static constexpr std::uint32_t m_iInitialWindDir = 0x2a;
        static constexpr std::uint32_t m_flInitialWindSpeed = 0x2c;
        static constexpr std::uint32_t m_location = 0x30;
        static constexpr std::uint32_t m_hEntOwner = 0x3c;
    };

    class C_InfoLadderDismount
    {
    public:
    };

    class C_GameRulesProxy
    {
    public:
    };

    class C_GameRules
    {
    public:
        static constexpr std::uint32_t m_nTotalPausedTicks = 0x30;
        static constexpr std::uint32_t m_nPauseStartTick = 0x34;
        static constexpr std::uint32_t m_bGamePaused = 0x38;
    };

    class C_MultiplayRules
    {
    public:
    };

    class C_SingleplayRules
    {
    public:
    };

    class C_SoundOpvarSetPointBase
    {
    public:
        static constexpr std::uint32_t m_iszStackName = 0x608;
        static constexpr std::uint32_t m_iszOperatorName = 0x610;
        static constexpr std::uint32_t m_iszOpvarName = 0x618;
        static constexpr std::uint32_t m_iOpvarIndex = 0x620;
        static constexpr std::uint32_t m_bUseAutoCompare = 0x624;
        static constexpr std::uint32_t m_bFastRefresh = 0x625;
    };

    class C_SoundOpvarSetPointEntity
    {
    public:
    };

    class C_SoundOpvarSetAABBEntity
    {
    public:
    };

    class C_SoundOpvarSetOBBEntity
    {
    public:
    };

    class C_SoundOpvarSetPathCornerEntity
    {
    public:
    };

    class C_SoundOpvarSetAutoRoomEntity
    {
    public:
    };

    class C_SoundOpvarSetOBBWindEntity
    {
    public:
    };

    class C_TeamplayRules
    {
    public:
    };

    class C_PortraitWorldCallbackHandler
    {
    public:
    };

    class C_EconItemView
    {
    public:
        static constexpr std::uint32_t m_bInventoryImageRgbaRequested = 0x60;
        static constexpr std::uint32_t m_bInventoryImageTriedCache = 0x61;
        static constexpr std::uint32_t m_nInventoryImageRgbaWidth = 0x80;
        static constexpr std::uint32_t m_nInventoryImageRgbaHeight = 0x84;
        static constexpr std::uint32_t m_szCurrentLoadCachedFileName = 0x88;
        static constexpr std::uint32_t m_bRestoreCustomMaterialAfterPrecache = 0x1b8;
        static constexpr std::uint32_t m_iItemDefinitionIndex = 0x1ba;
        static constexpr std::uint32_t m_iEntityQuality = 0x1bc;
        static constexpr std::uint32_t m_iEntityLevel = 0x1c0;
        static constexpr std::uint32_t m_iItemID = 0x1c8;
        static constexpr std::uint32_t m_iItemIDHigh = 0x1d0;
        static constexpr std::uint32_t m_iItemIDLow = 0x1d4;
        static constexpr std::uint32_t m_iAccountID = 0x1d8;
        static constexpr std::uint32_t m_iInventoryPosition = 0x1dc;
        static constexpr std::uint32_t m_bInitialized = 0x1e8;
        static constexpr std::uint32_t m_bDisallowSOC = 0x1e9;
        static constexpr std::uint32_t m_bIsStoreItem = 0x1ea;
        static constexpr std::uint32_t m_bIsTradeItem = 0x1eb;
        static constexpr std::uint32_t m_iEntityQuantity = 0x1ec;
        static constexpr std::uint32_t m_iRarityOverride = 0x1f0;
        static constexpr std::uint32_t m_iQualityOverride = 0x1f4;
        static constexpr std::uint32_t m_iOriginOverride = 0x1f8;
        static constexpr std::uint32_t m_ubStyleOverride = 0x1fc;
        static constexpr std::uint32_t m_unClientFlags = 0x1fd;
        static constexpr std::uint32_t m_AttributeList = 0x208;
        static constexpr std::uint32_t m_NetworkedDynamicAttributes = 0x280;
        static constexpr std::uint32_t m_szCustomName = 0x2f8;
        static constexpr std::uint32_t m_szCustomNameOverride = 0x399;
        static constexpr std::uint32_t m_bInitializedTags = 0x468;
    };

    class C_AttributeContainer
    {
    public:
        static constexpr std::uint32_t m_Item = 0x50;
        static constexpr std::uint32_t m_iExternalItemProviderRegisteredToken = 0x4c0;
        static constexpr std::uint32_t m_ullRegisteredAsItemID = 0x4c8;
    };

    class C_CSGameRules
    {
    public:
        static constexpr std::uint32_t m_bFreezePeriod = 0x40;
        static constexpr std::uint32_t m_bWarmupPeriod = 0x41;
        static constexpr std::uint32_t m_fWarmupPeriodEnd = 0x44;
        static constexpr std::uint32_t m_fWarmupPeriodStart = 0x48;
        static constexpr std::uint32_t m_bTerroristTimeOutActive = 0x4c;
        static constexpr std::uint32_t m_bCTTimeOutActive = 0x4d;
        static constexpr std::uint32_t m_flTerroristTimeOutRemaining = 0x50;
        static constexpr std::uint32_t m_flCTTimeOutRemaining = 0x54;
        static constexpr std::uint32_t m_nTerroristTimeOuts = 0x58;
        static constexpr std::uint32_t m_nCTTimeOuts = 0x5c;
        static constexpr std::uint32_t m_bTechnicalTimeOut = 0x60;
        static constexpr std::uint32_t m_bMatchWaitingForResume = 0x61;
        static constexpr std::uint32_t m_iFreezeTime = 0x64;
        static constexpr std::uint32_t m_iRoundTime = 0x68;
        static constexpr std::uint32_t m_fMatchStartTime = 0x6c;
        static constexpr std::uint32_t m_fRoundStartTime = 0x70;
        static constexpr std::uint32_t m_flRestartRoundTime = 0x74;
        static constexpr std::uint32_t m_bGameRestart = 0x78;
        static constexpr std::uint32_t m_flGameStartTime = 0x7c;
        static constexpr std::uint32_t m_timeUntilNextPhaseStarts = 0x80;
        static constexpr std::uint32_t m_gamePhase = 0x84;
        static constexpr std::uint32_t m_totalRoundsPlayed = 0x88;
        static constexpr std::uint32_t m_nRoundsPlayedThisPhase = 0x8c;
        static constexpr std::uint32_t m_nOvertimePlaying = 0x90;
        static constexpr std::uint32_t m_iHostagesRemaining = 0x94;
        static constexpr std::uint32_t m_bAnyHostageReached = 0x98;
        static constexpr std::uint32_t m_bMapHasBombTarget = 0x99;
        static constexpr std::uint32_t m_bMapHasRescueZone = 0x9a;
        static constexpr std::uint32_t m_bMapHasBuyZone = 0x9b;
        static constexpr std::uint32_t m_bIsQueuedMatchmaking = 0x9c;
        static constexpr std::uint32_t m_nQueuedMatchmakingMode = 0xa0;
        static constexpr std::uint32_t m_bIsValveDS = 0xa4;
        static constexpr std::uint32_t m_bLogoMap = 0xa5;
        static constexpr std::uint32_t m_bPlayAllStepSoundsOnServer = 0xa6;
        static constexpr std::uint32_t m_iSpectatorSlotCount = 0xa8;
        static constexpr std::uint32_t m_MatchDevice = 0xac;
        static constexpr std::uint32_t m_bHasMatchStarted = 0xb0;
        static constexpr std::uint32_t m_nNextMapInMapgroup = 0xb4;
        static constexpr std::uint32_t m_szTournamentEventName = 0xb8;
        static constexpr std::uint32_t m_szTournamentEventStage = 0x2b8;
        static constexpr std::uint32_t m_szMatchStatTxt = 0x4b8;
        static constexpr std::uint32_t m_szTournamentPredictionsTxt = 0x6b8;
        static constexpr std::uint32_t m_nTournamentPredictionsPct = 0x8b8;
        static constexpr std::uint32_t m_flCMMItemDropRevealStartTime = 0x8bc;
        static constexpr std::uint32_t m_flCMMItemDropRevealEndTime = 0x8c0;
        static constexpr std::uint32_t m_bIsDroppingItems = 0x8c4;
        static constexpr std::uint32_t m_bIsQuestEligible = 0x8c5;
        static constexpr std::uint32_t m_bIsHltvActive = 0x8c6;
        static constexpr std::uint32_t m_bBombPlanted = 0x8c7;
        static constexpr std::uint32_t m_arrProhibitedItemIndices = 0x8c8;
        static constexpr std::uint32_t m_arrTournamentActiveCasterAccounts = 0x990;
        static constexpr std::uint32_t m_numBestOfMaps = 0x9a0;
        static constexpr std::uint32_t m_nHalloweenMaskListSeed = 0x9a4;
        static constexpr std::uint32_t m_bBombDropped = 0x9a8;
        static constexpr std::uint32_t m_iRoundWinStatus = 0x9ac;
        static constexpr std::uint32_t m_eRoundWinReason = 0x9b0;
        static constexpr std::uint32_t m_bTCantBuy = 0x9b4;
        static constexpr std::uint32_t m_bCTCantBuy = 0x9b5;
        static constexpr std::uint32_t m_iMatchStats_RoundResults = 0x9b8;
        static constexpr std::uint32_t m_iMatchStats_PlayersAlive_CT = 0xa30;
        static constexpr std::uint32_t m_iMatchStats_PlayersAlive_T = 0xaa8;
        static constexpr std::uint32_t m_TeamRespawnWaveTimes = 0xb20;
        static constexpr std::uint32_t m_flNextRespawnWave = 0xba0;
        static constexpr std::uint32_t m_vMinimapMins = 0xc20;
        static constexpr std::uint32_t m_vMinimapMaxs = 0xc2c;
        static constexpr std::uint32_t m_MinimapVerticalSectionHeights = 0xc38;
        static constexpr std::uint32_t m_ullLocalMatchID = 0xc58;
        static constexpr std::uint32_t m_nEndMatchMapGroupVoteTypes = 0xc60;
        static constexpr std::uint32_t m_nEndMatchMapGroupVoteOptions = 0xc88;
        static constexpr std::uint32_t m_nEndMatchMapVoteWinner = 0xcb0;
        static constexpr std::uint32_t m_iNumConsecutiveCTLoses = 0xcb4;
        static constexpr std::uint32_t m_iNumConsecutiveTerroristLoses = 0xcb8;
        static constexpr std::uint32_t m_nMatchAbortedEarlyReason = 0xd78;
        static constexpr std::uint32_t m_bHasTriggeredRoundStartMusic = 0xd7c;
        static constexpr std::uint32_t m_bSwitchingTeamsAtRoundReset = 0xd7d;
        static constexpr std::uint32_t m_pGameModeRules = 0xd98;
        static constexpr std::uint32_t m_RetakeRules = 0xda0;
        static constexpr std::uint32_t m_nMatchEndCount = 0xef8;
        static constexpr std::uint32_t m_nTTeamIntroVariant = 0xefc;
        static constexpr std::uint32_t m_nCTTeamIntroVariant = 0xf00;
        static constexpr std::uint32_t m_bTeamIntroPeriod = 0xf04;
        static constexpr std::uint32_t m_iRoundEndWinnerTeam = 0xf08;
        static constexpr std::uint32_t m_eRoundEndReason = 0xf0c;
        static constexpr std::uint32_t m_bRoundEndShowTimerDefend = 0xf10;
        static constexpr std::uint32_t m_iRoundEndTimerTime = 0xf14;
        static constexpr std::uint32_t m_sRoundEndFunFactToken = 0xf18;
        static constexpr std::uint32_t m_iRoundEndFunFactPlayerSlot = 0xf20;
        static constexpr std::uint32_t m_iRoundEndFunFactData1 = 0xf24;
        static constexpr std::uint32_t m_iRoundEndFunFactData2 = 0xf28;
        static constexpr std::uint32_t m_iRoundEndFunFactData3 = 0xf2c;
        static constexpr std::uint32_t m_sRoundEndMessage = 0xf30;
        static constexpr std::uint32_t m_iRoundEndPlayerCount = 0xf38;
        static constexpr std::uint32_t m_bRoundEndNoMusic = 0xf3c;
        static constexpr std::uint32_t m_iRoundEndLegacy = 0xf40;
        static constexpr std::uint32_t m_nRoundEndCount = 0xf44;
        static constexpr std::uint32_t m_iRoundStartRoundNumber = 0xf48;
        static constexpr std::uint32_t m_nRoundStartCount = 0xf4c;
        static constexpr std::uint32_t m_flLastPerfSampleTime = 0x4f58;
    };

    class C_CSGameRulesProxy
    {
    public:
        static constexpr std::uint32_t m_pGameRules = 0x608;
    };

    class C_RetakeGameRules
    {
    public:
        static constexpr std::uint32_t m_nMatchSeed = 0x138;
        static constexpr std::uint32_t m_bBlockersPresent = 0x13c;
        static constexpr std::uint32_t m_bRoundInProgress = 0x13d;
        static constexpr std::uint32_t m_iFirstSecondHalfRound = 0x140;
        static constexpr std::uint32_t m_iBombSite = 0x144;
        static constexpr std::uint32_t m_hBombPlanter = 0x148;
    };

    class C_CSPlayerPawn
    {
    public:
        static constexpr std::uint32_t m_pBulletServices = 0x1660;
        static constexpr std::uint32_t m_pHostageServices = 0x1668;
        static constexpr std::uint32_t m_pBuyServices = 0x1670;
        static constexpr std::uint32_t m_pGlowServices = 0x1678;
        static constexpr std::uint32_t m_pActionTrackingServices = 0x1680;
        static constexpr std::uint32_t m_pDamageReactServices = 0x1688;
        static constexpr std::uint32_t m_flHealthShotBoostExpirationTime = 0x1690;
        static constexpr std::uint32_t m_flLastFiredWeaponTime = 0x1694;
        static constexpr std::uint32_t m_bHasFemaleVoice = 0x1698;
        static constexpr std::uint32_t m_flLandingTimeSeconds = 0x169c;
        static constexpr std::uint32_t m_flOldFallVelocity = 0x16a0;
        static constexpr std::uint32_t m_szLastPlaceName = 0x16a4;
        static constexpr std::uint32_t m_bPrevDefuser = 0x16b6;
        static constexpr std::uint32_t m_bPrevHelmet = 0x16b7;
        static constexpr std::uint32_t m_nPrevArmorVal = 0x16b8;
        static constexpr std::uint32_t m_nPrevGrenadeAmmoCount = 0x16bc;
        static constexpr std::uint32_t m_unPreviousWeaponHash = 0x16c0;
        static constexpr std::uint32_t m_unWeaponHash = 0x16c4;
        static constexpr std::uint32_t m_bInBuyZone = 0x16c8;
        static constexpr std::uint32_t m_bPreviouslyInBuyZone = 0x16c9;
        static constexpr std::uint32_t m_aimPunchAngle = 0x16cc;
        static constexpr std::uint32_t m_aimPunchAngleVel = 0x16d8;
        static constexpr std::uint32_t m_aimPunchTickBase = 0x16e4;
        static constexpr std::uint32_t m_aimPunchTickFraction = 0x16e8;
        static constexpr std::uint32_t m_bInLanding = 0x1710;
        static constexpr std::uint32_t m_flLandingStartTime = 0x1714;
        static constexpr std::uint32_t m_bInHostageRescueZone = 0x1718;
        static constexpr std::uint32_t m_bInBombZone = 0x1719;
        static constexpr std::uint32_t m_bIsBuyMenuOpen = 0x171a;
        static constexpr std::uint32_t m_flTimeOfLastInjury = 0x171c;
        static constexpr std::uint32_t m_flNextSprayDecalTime = 0x1720;
        static constexpr std::uint32_t m_iRetakesOffering = 0x1878;
        static constexpr std::uint32_t m_iRetakesOfferingCard = 0x187c;
        static constexpr std::uint32_t m_bRetakesHasDefuseKit = 0x1880;
        static constexpr std::uint32_t m_bRetakesMVPLastRound = 0x1881;
        static constexpr std::uint32_t m_iRetakesMVPBoostItem = 0x1884;
        static constexpr std::uint32_t m_RetakesMVPBoostExtraUtility = 0x1888;
        static constexpr std::uint32_t m_bNeedToReApplyGloves = 0x188d;
        static constexpr std::uint32_t m_EconGloves = 0x1890;
        static constexpr std::uint32_t m_nEconGlovesChanged = 0x1d00;
        static constexpr std::uint32_t m_bMustSyncRagdollState = 0x1d01;
        static constexpr std::uint32_t m_nRagdollDamageBone = 0x1d04;
        static constexpr std::uint32_t m_vRagdollDamageForce = 0x1d08;
        static constexpr std::uint32_t m_vRagdollDamagePosition = 0x1d14;
        static constexpr std::uint32_t m_szRagdollDamageWeaponName = 0x1d20;
        static constexpr std::uint32_t m_bRagdollDamageHeadshot = 0x1d60;
        static constexpr std::uint32_t m_vRagdollServerOrigin = 0x1d64;
        static constexpr std::uint32_t m_lastLandTime = 0x23e0;
        static constexpr std::uint32_t m_bOnGroundLastTick = 0x23e4;
        static constexpr std::uint32_t m_hHudModelArms = 0x2400;
        static constexpr std::uint32_t m_qDeathEyeAngles = 0x2404;
        static constexpr std::uint32_t m_bSkipOneHeadConstraintUpdate = 0x2410;
        static constexpr std::uint32_t m_bLeftHanded = 0x2411;
        static constexpr std::uint32_t m_fSwitchedHandednessTime = 0x2414;
        static constexpr std::uint32_t m_flViewmodelOffsetX = 0x2418;
        static constexpr std::uint32_t m_flViewmodelOffsetY = 0x241c;
        static constexpr std::uint32_t m_flViewmodelOffsetZ = 0x2420;
        static constexpr std::uint32_t m_flViewmodelFOV = 0x2424;
        static constexpr std::uint32_t m_vecPlayerPatchEconIndices = 0x2428;
        static constexpr std::uint32_t m_GunGameImmunityColor = 0x2460;
        static constexpr std::uint32_t m_vecBulletHitModels = 0x24b0;
        static constexpr std::uint32_t m_bIsWalking = 0x24c8;
        static constexpr std::uint32_t m_thirdPersonHeading = 0x24d0;
        static constexpr std::uint32_t m_flSlopeDropOffset = 0x2560;
        static constexpr std::uint32_t m_flSlopeDropHeight = 0x25d8;
        static constexpr std::uint32_t m_vHeadConstraintOffset = 0x2650;
        static constexpr std::uint32_t m_entitySpottedState = 0x26e0;
        static constexpr std::uint32_t m_bIsScoped = 0x26f8;
        static constexpr std::uint32_t m_bResumeZoom = 0x26f9;
        static constexpr std::uint32_t m_bIsDefusing = 0x26fa;
        static constexpr std::uint32_t m_bIsGrabbingHostage = 0x26fb;
        static constexpr std::uint32_t m_iBlockingUseActionInProgress = 0x26fc;
        static constexpr std::uint32_t m_flEmitSoundTime = 0x2700;
        static constexpr std::uint32_t m_bInNoDefuseArea = 0x2704;
        static constexpr std::uint32_t m_nWhichBombZone = 0x2708;
        static constexpr std::uint32_t m_iShotsFired = 0x270c;
        static constexpr std::uint32_t m_flFlinchStack = 0x2710;
        static constexpr std::uint32_t m_flVelocityModifier = 0x2714;
        static constexpr std::uint32_t m_flHitHeading = 0x2718;
        static constexpr std::uint32_t m_nHitBodyPart = 0x271c;
        static constexpr std::uint32_t m_bWaitForNoAttack = 0x2720;
        static constexpr std::uint32_t m_ignoreLadderJumpTime = 0x2724;
        static constexpr std::uint32_t m_bKilledByHeadshot = 0x2729;
        static constexpr std::uint32_t m_ArmorValue = 0x272c;
        static constexpr std::uint32_t m_unCurrentEquipmentValue = 0x2730;
        static constexpr std::uint32_t m_unRoundStartEquipmentValue = 0x2732;
        static constexpr std::uint32_t m_unFreezetimeEndEquipmentValue = 0x2734;
        static constexpr std::uint32_t m_nLastKillerIndex = 0x2738;
        static constexpr std::uint32_t m_bOldIsScoped = 0x273c;
        static constexpr std::uint32_t m_bHasDeathInfo = 0x273d;
        static constexpr std::uint32_t m_flDeathInfoTime = 0x2740;
        static constexpr std::uint32_t m_vecDeathInfoOrigin = 0x2744;
        static constexpr std::uint32_t m_grenadeParameterStashTime = 0x2754;
        static constexpr std::uint32_t m_bGrenadeParametersStashed = 0x2758;
        static constexpr std::uint32_t m_angStashedShootAngles = 0x275c;
        static constexpr std::uint32_t m_vecStashedGrenadeThrowPosition = 0x2768;
        static constexpr std::uint32_t m_vecStashedVelocity = 0x2774;
        static constexpr std::uint32_t m_angShootAngleHistory = 0x2780;
        static constexpr std::uint32_t m_vecThrowPositionHistory = 0x2798;
        static constexpr std::uint32_t m_vecVelocityHistory = 0x27b0;
        static constexpr std::uint32_t m_PredictedDamageTags = 0x27c8;
        static constexpr std::uint32_t m_nPrevHighestReceivedDamageTagTick = 0x2830;
        static constexpr std::uint32_t m_nHighestAppliedDamageTagTick = 0x2834;
        static constexpr std::uint32_t m_bShouldAutobuyDMWeapons = 0x3d6c;
        static constexpr std::uint32_t m_fImmuneToGunGameDamageTime = 0x3d70;
        static constexpr std::uint32_t m_bGunGameImmunity = 0x3d74;
        static constexpr std::uint32_t m_fImmuneToGunGameDamageTimeLast = 0x3d78;
        static constexpr std::uint32_t m_fMolotovDamageTime = 0x3d7c;
        static constexpr std::uint32_t m_vecLastAliveLocalVelocity = 0x3d84;
        static constexpr std::uint32_t m_fRenderingClipPlane = 0x3d90;
        static constexpr std::uint32_t m_nLastClipPlaneSetupFrame = 0x3da0;
        static constexpr std::uint32_t m_vecLastClipCameraPos = 0x3da4;
        static constexpr std::uint32_t m_vecLastClipCameraForward = 0x3db0;
        static constexpr std::uint32_t m_bClipHitStaticWorld = 0x3dbc;
        static constexpr std::uint32_t m_bCachedPlaneIsValid = 0x3dbd;
        static constexpr std::uint32_t m_pClippingWeapon = 0x3dc0;
        static constexpr std::uint32_t m_nPlayerInfernoBodyFx = 0x3dc8;
        static constexpr std::uint32_t m_angEyeAngles = 0x3dd0;
        static constexpr std::uint32_t m_arrOldEyeAnglesTimes = 0x3e60;
        static constexpr std::uint32_t m_arrOldEyeAngles = 0x3e70;
        static constexpr std::uint32_t m_angEyeAnglesVelocity = 0x3ea0;
        static constexpr std::uint32_t m_iIDEntIndex = 0x3eac;
        static constexpr std::uint32_t m_delayTargetIDTimer = 0x3eb0;
        static constexpr std::uint32_t m_iTargetItemEntIdx = 0x3ec8;
        static constexpr std::uint32_t m_iOldIDEntIndex = 0x3ecc;
        static constexpr std::uint32_t m_holdTargetIDTimer = 0x3ed0;
    };

    class C_CSGO_TeamPreviewCharacterPosition
    {
    public:
        static constexpr std::uint32_t m_nVariant = 0x608;
        static constexpr std::uint32_t m_nRandom = 0x60c;
        static constexpr std::uint32_t m_nOrdinal = 0x610;
        static constexpr std::uint32_t m_sWeaponName = 0x618;
        static constexpr std::uint32_t m_xuid = 0x620;
        static constexpr std::uint32_t m_agentItem = 0x628;
        static constexpr std::uint32_t m_glovesItem = 0xa98;
        static constexpr std::uint32_t m_weaponItem = 0xf08;
    };

    class C_CSGO_TeamSelectCharacterPosition
    {
    public:
    };

    class C_CSGO_TeamSelectTerroristPosition
    {
    public:
    };

    class C_CSGO_TeamSelectCounterTerroristPosition
    {
    public:
    };

    class C_CSGO_TeamIntroCharacterPosition
    {
    public:
    };

    class C_CSGO_TeamIntroTerroristPosition
    {
    public:
    };

    class C_CSGO_TeamIntroCounterTerroristPosition
    {
    public:
    };

    class C_CSMinimapBoundary
    {
    public:
    };

    class C_CSPetPlacement
    {
    public:
    };

    class C_PlayerPing
    {
    public:
        static constexpr std::uint32_t m_hPlayer = 0x638;
        static constexpr std::uint32_t m_hPingedEntity = 0x63c;
        static constexpr std::uint32_t m_iType = 0x640;
        static constexpr std::uint32_t m_bUrgent = 0x644;
        static constexpr std::uint32_t m_szPlaceName = 0x645;
    };

    class C_CSPlayerResource
    {
    public:
        static constexpr std::uint32_t m_bHostageAlive = 0x608;
        static constexpr std::uint32_t m_isHostageFollowingSomeone = 0x614;
        static constexpr std::uint32_t m_iHostageEntityIDs = 0x620;
        static constexpr std::uint32_t m_bombsiteCenterA = 0x650;
        static constexpr std::uint32_t m_bombsiteCenterB = 0x65c;
        static constexpr std::uint32_t m_hostageRescueX = 0x668;
        static constexpr std::uint32_t m_hostageRescueY = 0x678;
        static constexpr std::uint32_t m_hostageRescueZ = 0x688;
        static constexpr std::uint32_t m_bEndMatchNextMapAllVoted = 0x698;
        static constexpr std::uint32_t m_foundGoalPositions = 0x699;
    };

    class C_IronSightController
    {
    public:
        static constexpr std::uint32_t m_bIronSightAvailable = 0x10;
        static constexpr std::uint32_t m_flIronSightAmount = 0x14;
        static constexpr std::uint32_t m_flIronSightAmountGained = 0x18;
        static constexpr std::uint32_t m_flIronSightAmountBiased = 0x1c;
        static constexpr std::uint32_t m_flIronSightAmount_Interpolated = 0x20;
        static constexpr std::uint32_t m_flIronSightAmountGained_Interpolated = 0x24;
        static constexpr std::uint32_t m_flIronSightAmountBiased_Interpolated = 0x28;
        static constexpr std::uint32_t m_flInterpolationLastUpdated = 0x2c;
        static constexpr std::uint32_t m_angDeltaAverage = 0x30;
        static constexpr std::uint32_t m_angViewLast = 0x90;
        static constexpr std::uint32_t m_vecDotCoords = 0x9c;
        static constexpr std::uint32_t m_flFiringInaccuracyExtraWidthMultiplier = 0xa4;
        static constexpr std::uint32_t m_flSpeedRatio = 0xa8;
    };

    class C_EnvWindClientside
    {
    public:
        static constexpr std::uint32_t m_EnvWindShared = 0x608;
    };

    class C_GlobalLight
    {
    public:
        static constexpr std::uint32_t m_WindClothForceHandle = 0xad0;
    };

    class C_CSGO_PreviewPlayer_GraphController
    {
    public:
        static constexpr std::uint32_t m_pszCharacterMode = 0x90;
        static constexpr std::uint32_t m_pszTeamPreviewVariant = 0xc0;
        static constexpr std::uint32_t m_pszTeamPreviewPosition = 0xf0;
        static constexpr std::uint32_t m_pszEndOfMatchCelebration = 0x120;
        static constexpr std::uint32_t m_nTeamPreviewRandom = 0x150;
        static constexpr std::uint32_t m_pszWeaponState = 0x178;
        static constexpr std::uint32_t m_pszWeaponType = 0x1a8;
        static constexpr std::uint32_t m_bCT = 0x1d8;
    };

    class C_CSGO_MapPreviewCameraPathNode
    {
    public:
        static constexpr std::uint32_t m_szParentPathUniqueID = 0x608;
        static constexpr std::uint32_t m_nPathIndex = 0x610;
        static constexpr std::uint32_t m_vInTangentLocal = 0x614;
        static constexpr std::uint32_t m_vOutTangentLocal = 0x620;
        static constexpr std::uint32_t m_flFOV = 0x62c;
        static constexpr std::uint32_t m_flCameraSpeed = 0x630;
        static constexpr std::uint32_t m_flEaseIn = 0x634;
        static constexpr std::uint32_t m_flEaseOut = 0x638;
        static constexpr std::uint32_t m_vInTangentWorld = 0x63c;
        static constexpr std::uint32_t m_vOutTangentWorld = 0x648;
    };

    class C_CSGO_MapPreviewCameraPath
    {
    public:
        static constexpr std::uint32_t m_flZFar = 0x608;
        static constexpr std::uint32_t m_flZNear = 0x60c;
        static constexpr std::uint32_t m_bLoop = 0x610;
        static constexpr std::uint32_t m_bVerticalFOV = 0x611;
        static constexpr std::uint32_t m_bConstantSpeed = 0x612;
        static constexpr std::uint32_t m_flDuration = 0x614;
        static constexpr std::uint32_t m_flPathLength = 0x658;
        static constexpr std::uint32_t m_flPathDuration = 0x65c;
        static constexpr std::uint32_t m_bDofEnabled = 0x674;
        static constexpr std::uint32_t m_flDofNearBlurry = 0x678;
        static constexpr std::uint32_t m_flDofNearCrisp = 0x67c;
        static constexpr std::uint32_t m_flDofFarCrisp = 0x680;
        static constexpr std::uint32_t m_flDofFarBlurry = 0x684;
        static constexpr std::uint32_t m_flDofTiltToGround = 0x688;
    };

    class C_VoteController
    {
    public:
        static constexpr std::uint32_t m_iActiveIssueIndex = 0x618;
        static constexpr std::uint32_t m_iOnlyTeamToVote = 0x61c;
        static constexpr std::uint32_t m_nVoteOptionCount = 0x620;
        static constexpr std::uint32_t m_nPotentialVotes = 0x634;
        static constexpr std::uint32_t m_bVotesDirty = 0x638;
        static constexpr std::uint32_t m_bTypeDirty = 0x639;
        static constexpr std::uint32_t m_bIsYesNoVote = 0x63a;
    };

    class C_MapVetoPickController
    {
    public:
        static constexpr std::uint32_t m_nDraftType = 0x618;
        static constexpr std::uint32_t m_nTeamWinningCoinToss = 0x61c;
        static constexpr std::uint32_t m_nTeamWithFirstChoice = 0x620;
        static constexpr std::uint32_t m_nVoteMapIdsList = 0x720;
        static constexpr std::uint32_t m_nAccountIDs = 0x73c;
        static constexpr std::uint32_t m_nMapId0 = 0x83c;
        static constexpr std::uint32_t m_nMapId1 = 0x93c;
        static constexpr std::uint32_t m_nMapId2 = 0xa3c;
        static constexpr std::uint32_t m_nMapId3 = 0xb3c;
        static constexpr std::uint32_t m_nMapId4 = 0xc3c;
        static constexpr std::uint32_t m_nMapId5 = 0xd3c;
        static constexpr std::uint32_t m_nStartingSide0 = 0xe3c;
        static constexpr std::uint32_t m_nCurrentPhase = 0xf3c;
        static constexpr std::uint32_t m_nPhaseStartTick = 0xf40;
        static constexpr std::uint32_t m_nPhaseDurationTicks = 0xf44;
        static constexpr std::uint32_t m_nPostDataUpdateTick = 0xf48;
        static constexpr std::uint32_t m_bDisabledHud = 0xf4c;
    };

    class C_CSGO_TeamPreviewCamera
    {
    public:
        static constexpr std::uint32_t m_nVariant = 0x690;
    };

    class C_CSGO_TeamSelectCamera
    {
    public:
    };

    class C_CSGO_TerroristTeamIntroCamera
    {
    public:
    };

    class C_CSGO_TerroristWingmanIntroCamera
    {
    public:
    };

    class C_CSGO_CounterTerroristTeamIntroCamera
    {
    public:
    };

    class C_CSGO_CounterTerroristWingmanIntroCamera
    {
    public:
    };

    class C_CSGO_EndOfMatchCamera
    {
    public:
    };

    class C_CSGO_EndOfMatchCharacterPosition
    {
    public:
    };

    class C_CSGO_EndOfMatchLineupEndpoint
    {
    public:
    };

    class C_CSGO_EndOfMatchLineupStart
    {
    public:
    };

    class C_CSGO_EndOfMatchLineupEnd
    {
    public:
    };

    class C_CsmFovOverride
    {
    public:
        static constexpr std::uint32_t m_cameraName = 0x608;
        static constexpr std::uint32_t m_flCsmFovOverrideValue = 0x610;
    };

    class C_PointEntity
    {
    public:
    };

    class C_EnvCombinedLightProbeVolume
    {
    public:
        static constexpr std::uint32_t m_Entity_Color = 0x1680;
        static constexpr std::uint32_t m_Entity_flBrightness = 0x1684;
        static constexpr std::uint32_t m_Entity_hCubemapTexture = 0x1688;
        static constexpr std::uint32_t m_Entity_bCustomCubemapTexture = 0x1690;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_AmbientCube = 0x1698;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SDF = 0x16a0;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_DC = 0x16a8;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_R = 0x16b0;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_G = 0x16b8;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_B = 0x16c0;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightIndicesTexture = 0x16c8;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightScalarsTexture = 0x16d0;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightShadowsTexture = 0x16d8;
        static constexpr std::uint32_t m_Entity_vBoxMins = 0x16e0;
        static constexpr std::uint32_t m_Entity_vBoxMaxs = 0x16ec;
        static constexpr std::uint32_t m_Entity_bMoveable = 0x16f8;
        static constexpr std::uint32_t m_Entity_nHandshake = 0x16fc;
        static constexpr std::uint32_t m_Entity_nEnvCubeMapArrayIndex = 0x1700;
        static constexpr std::uint32_t m_Entity_nPriority = 0x1704;
        static constexpr std::uint32_t m_Entity_bStartDisabled = 0x1708;
        static constexpr std::uint32_t m_Entity_flEdgeFadeDist = 0x170c;
        static constexpr std::uint32_t m_Entity_vEdgeFadeDists = 0x1710;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeX = 0x171c;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeY = 0x1720;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeZ = 0x1724;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasX = 0x1728;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasY = 0x172c;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasZ = 0x1730;
        static constexpr std::uint32_t m_Entity_bEnabled = 0x1749;
    };

    class C_EnvCubemap
    {
    public:
        static constexpr std::uint32_t m_Entity_hCubemapTexture = 0x688;
        static constexpr std::uint32_t m_Entity_bCustomCubemapTexture = 0x690;
        static constexpr std::uint32_t m_Entity_flInfluenceRadius = 0x694;
        static constexpr std::uint32_t m_Entity_vBoxProjectMins = 0x698;
        static constexpr std::uint32_t m_Entity_vBoxProjectMaxs = 0x6a4;
        static constexpr std::uint32_t m_Entity_bMoveable = 0x6b0;
        static constexpr std::uint32_t m_Entity_nHandshake = 0x6b4;
        static constexpr std::uint32_t m_Entity_nEnvCubeMapArrayIndex = 0x6b8;
        static constexpr std::uint32_t m_Entity_nPriority = 0x6bc;
        static constexpr std::uint32_t m_Entity_flEdgeFadeDist = 0x6c0;
        static constexpr std::uint32_t m_Entity_vEdgeFadeDists = 0x6c4;
        static constexpr std::uint32_t m_Entity_flDiffuseScale = 0x6d0;
        static constexpr std::uint32_t m_Entity_bStartDisabled = 0x6d4;
        static constexpr std::uint32_t m_Entity_bDefaultEnvMap = 0x6d5;
        static constexpr std::uint32_t m_Entity_bDefaultSpecEnvMap = 0x6d6;
        static constexpr std::uint32_t m_Entity_bIndoorCubeMap = 0x6d7;
        static constexpr std::uint32_t m_Entity_bCopyDiffuseFromDefaultCubemap = 0x6d8;
        static constexpr std::uint32_t m_Entity_bEnabled = 0x6e8;
    };

    class C_EnvCubemapBox
    {
    public:
    };

    class C_EnvCubemapFog
    {
    public:
        static constexpr std::uint32_t m_flEndDistance = 0x608;
        static constexpr std::uint32_t m_flStartDistance = 0x60c;
        static constexpr std::uint32_t m_flFogFalloffExponent = 0x610;
        static constexpr std::uint32_t m_bHeightFogEnabled = 0x614;
        static constexpr std::uint32_t m_flFogHeightWidth = 0x618;
        static constexpr std::uint32_t m_flFogHeightEnd = 0x61c;
        static constexpr std::uint32_t m_flFogHeightStart = 0x620;
        static constexpr std::uint32_t m_flFogHeightExponent = 0x624;
        static constexpr std::uint32_t m_flLODBias = 0x628;
        static constexpr std::uint32_t m_bActive = 0x62c;
        static constexpr std::uint32_t m_bStartDisabled = 0x62d;
        static constexpr std::uint32_t m_flFogMaxOpacity = 0x630;
        static constexpr std::uint32_t m_nCubemapSourceType = 0x634;
        static constexpr std::uint32_t m_hSkyMaterial = 0x638;
        static constexpr std::uint32_t m_iszSkyEntity = 0x640;
        static constexpr std::uint32_t m_hFogCubemapTexture = 0x648;
        static constexpr std::uint32_t m_bHasHeightFogEnd = 0x650;
        static constexpr std::uint32_t m_bFirstTime = 0x651;
    };

    class C_GradientFog
    {
    public:
        static constexpr std::uint32_t m_hGradientFogTexture = 0x608;
        static constexpr std::uint32_t m_flFogStartDistance = 0x610;
        static constexpr std::uint32_t m_flFogEndDistance = 0x614;
        static constexpr std::uint32_t m_bHeightFogEnabled = 0x618;
        static constexpr std::uint32_t m_flFogStartHeight = 0x61c;
        static constexpr std::uint32_t m_flFogEndHeight = 0x620;
        static constexpr std::uint32_t m_flFarZ = 0x624;
        static constexpr std::uint32_t m_flFogMaxOpacity = 0x628;
        static constexpr std::uint32_t m_flFogFalloffExponent = 0x62c;
        static constexpr std::uint32_t m_flFogVerticalExponent = 0x630;
        static constexpr std::uint32_t m_fogColor = 0x634;
        static constexpr std::uint32_t m_flFogStrength = 0x638;
        static constexpr std::uint32_t m_flFadeTime = 0x63c;
        static constexpr std::uint32_t m_bStartDisabled = 0x640;
        static constexpr std::uint32_t m_bIsEnabled = 0x641;
        static constexpr std::uint32_t m_bGradientFogNeedsTextures = 0x642;
    };

    class C_EnvLightProbeVolume
    {
    public:
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_AmbientCube = 0x1600;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SDF = 0x1608;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_DC = 0x1610;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_R = 0x1618;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_G = 0x1620;
        static constexpr std::uint32_t m_Entity_hLightProbeTexture_SH2_B = 0x1628;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightIndicesTexture = 0x1630;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightScalarsTexture = 0x1638;
        static constexpr std::uint32_t m_Entity_hLightProbeDirectLightShadowsTexture = 0x1640;
        static constexpr std::uint32_t m_Entity_vBoxMins = 0x1648;
        static constexpr std::uint32_t m_Entity_vBoxMaxs = 0x1654;
        static constexpr std::uint32_t m_Entity_bMoveable = 0x1660;
        static constexpr std::uint32_t m_Entity_nHandshake = 0x1664;
        static constexpr std::uint32_t m_Entity_nPriority = 0x1668;
        static constexpr std::uint32_t m_Entity_bStartDisabled = 0x166c;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeX = 0x1670;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeY = 0x1674;
        static constexpr std::uint32_t m_Entity_nLightProbeSizeZ = 0x1678;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasX = 0x167c;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasY = 0x1680;
        static constexpr std::uint32_t m_Entity_nLightProbeAtlasZ = 0x1684;
        static constexpr std::uint32_t m_Entity_bEnabled = 0x1691;
    };

    class C_PlayerVisibility
    {
    public:
        static constexpr std::uint32_t m_flVisibilityStrength = 0x608;
        static constexpr std::uint32_t m_flFogDistanceMultiplier = 0x60c;
        static constexpr std::uint32_t m_flFogMaxDensityMultiplier = 0x610;
        static constexpr std::uint32_t m_flFadeTime = 0x614;
        static constexpr std::uint32_t m_bStartDisabled = 0x618;
        static constexpr std::uint32_t m_bIsEnabled = 0x619;
    };

    class C_EnvVolumetricFogController
    {
    public:
        static constexpr std::uint32_t m_flScattering = 0x608;
        static constexpr std::uint32_t m_TintColor = 0x60c;
        static constexpr std::uint32_t m_flAnisotropy = 0x610;
        static constexpr std::uint32_t m_flFadeSpeed = 0x614;
        static constexpr std::uint32_t m_flDrawDistance = 0x618;
        static constexpr std::uint32_t m_flFadeInStart = 0x61c;
        static constexpr std::uint32_t m_flFadeInEnd = 0x620;
        static constexpr std::uint32_t m_flIndirectStrength = 0x624;
        static constexpr std::uint32_t m_nVolumeDepth = 0x628;
        static constexpr std::uint32_t m_fFirstVolumeSliceThickness = 0x62c;
        static constexpr std::uint32_t m_nIndirectTextureDimX = 0x630;
        static constexpr std::uint32_t m_nIndirectTextureDimY = 0x634;
        static constexpr std::uint32_t m_nIndirectTextureDimZ = 0x638;
        static constexpr std::uint32_t m_vBoxMins = 0x63c;
        static constexpr std::uint32_t m_vBoxMaxs = 0x648;
        static constexpr std::uint32_t m_bActive = 0x654;
        static constexpr std::uint32_t m_flStartAnisoTime = 0x658;
        static constexpr std::uint32_t m_flStartScatterTime = 0x65c;
        static constexpr std::uint32_t m_flStartDrawDistanceTime = 0x660;
        static constexpr std::uint32_t m_flStartAnisotropy = 0x664;
        static constexpr std::uint32_t m_flStartScattering = 0x668;
        static constexpr std::uint32_t m_flStartDrawDistance = 0x66c;
        static constexpr std::uint32_t m_flDefaultAnisotropy = 0x670;
        static constexpr std::uint32_t m_flDefaultScattering = 0x674;
        static constexpr std::uint32_t m_flDefaultDrawDistance = 0x678;
        static constexpr std::uint32_t m_bStartDisabled = 0x67c;
        static constexpr std::uint32_t m_bEnableIndirect = 0x67d;
        static constexpr std::uint32_t m_bIsMaster = 0x67e;
        static constexpr std::uint32_t m_hFogIndirectTexture = 0x680;
        static constexpr std::uint32_t m_nForceRefreshCount = 0x688;
        static constexpr std::uint32_t m_fNoiseSpeed = 0x68c;
        static constexpr std::uint32_t m_fNoiseStrength = 0x690;
        static constexpr std::uint32_t m_vNoiseScale = 0x694;
        static constexpr std::uint32_t m_fWindSpeed = 0x6a0;
        static constexpr std::uint32_t m_vWindDirection = 0x6a4;
        static constexpr std::uint32_t m_bFirstTime = 0x6b0;
    };

    class C_EnvVolumetricFogVolume
    {
    public:
        static constexpr std::uint32_t m_bActive = 0x608;
        static constexpr std::uint32_t m_vBoxMins = 0x60c;
        static constexpr std::uint32_t m_vBoxMaxs = 0x618;
        static constexpr std::uint32_t m_bStartDisabled = 0x624;
        static constexpr std::uint32_t m_bIndirectUseLPVs = 0x625;
        static constexpr std::uint32_t m_flStrength = 0x628;
        static constexpr std::uint32_t m_nFalloffShape = 0x62c;
        static constexpr std::uint32_t m_flFalloffExponent = 0x630;
        static constexpr std::uint32_t m_flHeightFogDepth = 0x634;
        static constexpr std::uint32_t m_fHeightFogEdgeWidth = 0x638;
        static constexpr std::uint32_t m_fIndirectLightStrength = 0x63c;
        static constexpr std::uint32_t m_fSunLightStrength = 0x640;
        static constexpr std::uint32_t m_fNoiseStrength = 0x644;
        static constexpr std::uint32_t m_TintColor = 0x648;
        static constexpr std::uint32_t m_bOverrideTintColor = 0x64c;
        static constexpr std::uint32_t m_bOverrideIndirectLightStrength = 0x64d;
        static constexpr std::uint32_t m_bOverrideSunLightStrength = 0x64e;
        static constexpr std::uint32_t m_bOverrideNoiseStrength = 0x64f;
    };

    class C_EnvWindController
    {
    public:
        static constexpr std::uint32_t m_EnvWindShared = 0x608;
        static constexpr std::uint32_t m_fDirectionVariation = 0x700;
        static constexpr std::uint32_t m_fSpeedVariation = 0x704;
        static constexpr std::uint32_t m_fTurbulence = 0x708;
        static constexpr std::uint32_t m_fVolumeHalfExtentXY = 0x70c;
        static constexpr std::uint32_t m_fVolumeHalfExtentZ = 0x710;
        static constexpr std::uint32_t m_nVolumeResolutionXY = 0x714;
        static constexpr std::uint32_t m_nVolumeResolutionZ = 0x718;
        static constexpr std::uint32_t m_nClipmapLevels = 0x71c;
        static constexpr std::uint32_t m_bIsMaster = 0x720;
        static constexpr std::uint32_t m_bFirstTime = 0x721;
    };

    class C_EnvWindVolume
    {
    public:
        static constexpr std::uint32_t m_bActive = 0x608;
        static constexpr std::uint32_t m_vBoxMins = 0x60c;
        static constexpr std::uint32_t m_vBoxMaxs = 0x618;
        static constexpr std::uint32_t m_bStartDisabled = 0x624;
        static constexpr std::uint32_t m_nShape = 0x628;
        static constexpr std::uint32_t m_fWindSpeedMultiplier = 0x62c;
        static constexpr std::uint32_t m_fWindTurbulenceMultiplier = 0x630;
        static constexpr std::uint32_t m_fWindSpeedVariationMultiplier = 0x634;
        static constexpr std::uint32_t m_fWindDirectionVariationMultiplier = 0x638;
    };

    class C_InfoVisibilityBox
    {
    public:
        static constexpr std::uint32_t m_nMode = 0x60c;
        static constexpr std::uint32_t m_vBoxSize = 0x610;
        static constexpr std::uint32_t m_bEnabled = 0x61c;
    };

    class C_PointCameraVFOV
    {
    public:
        static constexpr std::uint32_t m_flVerticalFOV = 0x668;
    };

    class C_SoundAreaEntityBase
    {
    public:
        static constexpr std::uint32_t m_bDisabled = 0x608;
        static constexpr std::uint32_t m_bWasEnabled = 0x610;
        static constexpr std::uint32_t m_iszSoundAreaType = 0x618;
        static constexpr std::uint32_t m_vPos = 0x620;
    };

    class C_SoundAreaEntitySphere
    {
    public:
        static constexpr std::uint32_t m_flRadius = 0x630;
    };

    class C_SoundAreaEntityOrientedBox
    {
    public:
        static constexpr std::uint32_t m_vMin = 0x630;
        static constexpr std::uint32_t m_vMax = 0x63c;
    };

    class C_SoundEventEntity
    {
    public:
        static constexpr std::uint32_t m_bStartOnSpawn = 0x608;
        static constexpr std::uint32_t m_bToLocalPlayer = 0x609;
        static constexpr std::uint32_t m_bStopOnNew = 0x60a;
        static constexpr std::uint32_t m_bSaveRestore = 0x60b;
        static constexpr std::uint32_t m_bSavedIsPlaying = 0x60c;
        static constexpr std::uint32_t m_flSavedElapsedTime = 0x610;
        static constexpr std::uint32_t m_iszSourceEntityName = 0x618;
        static constexpr std::uint32_t m_iszAttachmentName = 0x620;
        static constexpr std::uint32_t m_onGUIDChanged = 0x628;
        static constexpr std::uint32_t m_onSoundFinished = 0x648;
        static constexpr std::uint32_t m_flClientCullRadius = 0x660;
        static constexpr std::uint32_t m_iszSoundName = 0x690;
        static constexpr std::uint32_t m_hSource = 0x6ac;
        static constexpr std::uint32_t m_nEntityIndexSelection = 0x6b0;
        static constexpr std::uint32_t m_bClientSideOnly = 0x0;
    };

    class C_SoundEventEntityAlias_snd_event_point
    {
    public:
    };

    class C_SoundEventAABBEntity
    {
    public:
        static constexpr std::uint32_t m_vMins = 0x6b8;
        static constexpr std::uint32_t m_vMaxs = 0x6c4;
    };

    class C_SoundEventOBBEntity
    {
    public:
        static constexpr std::uint32_t m_vMins = 0x6b8;
        static constexpr std::uint32_t m_vMaxs = 0x6c4;
    };

    class C_SoundEventSphereEntity
    {
    public:
        static constexpr std::uint32_t m_flRadius = 0x6b8;
    };

    class C_SoundEventConeEntity
    {
    public:
        static constexpr std::uint32_t m_flEmitterAngle = 0x6b8;
        static constexpr std::uint32_t m_flSweetSpotAngle = 0x6bc;
        static constexpr std::uint32_t m_flAttenMin = 0x6c0;
        static constexpr std::uint32_t m_flAttenMax = 0x6c4;
        static constexpr std::uint32_t m_iszParameterName = 0x6c8;
    };

    class C_SoundEventPathCornerEntity
    {
    public:
        static constexpr std::uint32_t m_vecCornerPairsNetworked = 0x6b8;
    };

    class C_BasePlayerPawn
    {
    public:
        static constexpr std::uint32_t m_pWeaponServices = 0x13d8;
        static constexpr std::uint32_t m_pItemServices = 0x13e0;
        static constexpr std::uint32_t m_pAutoaimServices = 0x13e8;
        static constexpr std::uint32_t m_pObserverServices = 0x13f0;
        static constexpr std::uint32_t m_pWaterServices = 0x13f8;
        static constexpr std::uint32_t m_pUseServices = 0x1400;
        static constexpr std::uint32_t m_pFlashlightServices = 0x1408;
        static constexpr std::uint32_t m_pCameraServices = 0x1410;
        static constexpr std::uint32_t m_pMovementServices = 0x1418;
        static constexpr std::uint32_t m_ServerViewAngleChanges = 0x1428;
        static constexpr std::uint32_t v_angle = 0x1490;
        static constexpr std::uint32_t v_anglePrevious = 0x149c;
        static constexpr std::uint32_t m_iHideHUD = 0x14a8;
        static constexpr std::uint32_t m_skybox3d = 0x14b0;
        static constexpr std::uint32_t m_flDeathTime = 0x1540;
        static constexpr std::uint32_t m_vecPredictionError = 0x1544;
        static constexpr std::uint32_t m_flPredictionErrorTime = 0x1550;
        static constexpr std::uint32_t m_vecLastCameraSetupLocalOrigin = 0x1570;
        static constexpr std::uint32_t m_flLastCameraSetupTime = 0x157c;
        static constexpr std::uint32_t m_flFOVSensitivityAdjust = 0x1580;
        static constexpr std::uint32_t m_flMouseSensitivity = 0x1584;
        static constexpr std::uint32_t m_vOldOrigin = 0x1588;
        static constexpr std::uint32_t m_flOldSimulationTime = 0x1594;
        static constexpr std::uint32_t m_nLastExecutedCommandNumber = 0x1598;
        static constexpr std::uint32_t m_nLastExecutedCommandTick = 0x159c;
        static constexpr std::uint32_t m_hController = 0x15a0;
        static constexpr std::uint32_t m_hDefaultController = 0x15a4;
        static constexpr std::uint32_t m_bIsSwappingToPredictableController = 0x15a8;
    };

    class C_Team
    {
    public:
        static constexpr std::uint32_t m_aPlayerControllers = 0x608;
        static constexpr std::uint32_t m_aPlayers = 0x620;
        static constexpr std::uint32_t m_iScore = 0x638;
        static constexpr std::uint32_t m_szTeamname = 0x63c;
    };

    class C_ModelPointEntity
    {
    public:
    };

    class C_PathParticleRope
    {
    public:
        static constexpr std::uint32_t m_bStartActive = 0x610;
        static constexpr std::uint32_t m_flMaxSimulationTime = 0x614;
        static constexpr std::uint32_t m_iszEffectName = 0x618;
        static constexpr std::uint32_t m_PathNodes_Name = 0x620;
        static constexpr std::uint32_t m_flParticleSpacing = 0x638;
        static constexpr std::uint32_t m_flSlack = 0x63c;
        static constexpr std::uint32_t m_flRadius = 0x640;
        static constexpr std::uint32_t m_ColorTint = 0x644;
        static constexpr std::uint32_t m_nEffectState = 0x648;
        static constexpr std::uint32_t m_iEffectIndex = 0x650;
        static constexpr std::uint32_t m_PathNodes_Position = 0x658;
        static constexpr std::uint32_t m_PathNodes_TangentIn = 0x670;
        static constexpr std::uint32_t m_PathNodes_TangentOut = 0x688;
        static constexpr std::uint32_t m_PathNodes_Color = 0x6a0;
        static constexpr std::uint32_t m_PathNodes_PinEnabled = 0x6b8;
        static constexpr std::uint32_t m_PathNodes_RadiusScale = 0x6d0;
    };

    class C_PathParticleRopeAlias_path_particle_rope_clientside
    {
    public:
    };

    class C_DynamicLight
    {
    public:
        static constexpr std::uint32_t m_Flags = 0xe88;
        static constexpr std::uint32_t m_LightStyle = 0xe89;
        static constexpr std::uint32_t m_Radius = 0xe8c;
        static constexpr std::uint32_t m_Exponent = 0xe90;
        static constexpr std::uint32_t m_InnerAngle = 0xe94;
        static constexpr std::uint32_t m_OuterAngle = 0xe98;
        static constexpr std::uint32_t m_SpotRadius = 0xe9c;
    };

    class C_FuncTrackTrain
    {
    public:
        static constexpr std::uint32_t m_nLongAxis = 0xe88;
        static constexpr std::uint32_t m_flRadius = 0xe8c;
        static constexpr std::uint32_t m_flLineLength = 0xe90;
    };

    class C_SpotlightEnd
    {
    public:
        static constexpr std::uint32_t m_flLightScale = 0xe88;
        static constexpr std::uint32_t m_Radius = 0xe8c;
    };

    class C_PointValueRemapper
    {
    public:
        static constexpr std::uint32_t m_bDisabled = 0x608;
        static constexpr std::uint32_t m_bDisabledOld = 0x609;
        static constexpr std::uint32_t m_bUpdateOnClient = 0x60a;
        static constexpr std::uint32_t m_nInputType = 0x60c;
        static constexpr std::uint32_t m_hRemapLineStart = 0x610;
        static constexpr std::uint32_t m_hRemapLineEnd = 0x614;
        static constexpr std::uint32_t m_flMaximumChangePerSecond = 0x618;
        static constexpr std::uint32_t m_flDisengageDistance = 0x61c;
        static constexpr std::uint32_t m_flEngageDistance = 0x620;
        static constexpr std::uint32_t m_bRequiresUseKey = 0x624;
        static constexpr std::uint32_t m_nOutputType = 0x628;
        static constexpr std::uint32_t m_hOutputEntities = 0x630;
        static constexpr std::uint32_t m_nHapticsType = 0x648;
        static constexpr std::uint32_t m_nMomentumType = 0x64c;
        static constexpr std::uint32_t m_flMomentumModifier = 0x650;
        static constexpr std::uint32_t m_flSnapValue = 0x654;
        static constexpr std::uint32_t m_flCurrentMomentum = 0x658;
        static constexpr std::uint32_t m_nRatchetType = 0x65c;
        static constexpr std::uint32_t m_flRatchetOffset = 0x660;
        static constexpr std::uint32_t m_flInputOffset = 0x664;
        static constexpr std::uint32_t m_bEngaged = 0x668;
        static constexpr std::uint32_t m_bFirstUpdate = 0x669;
        static constexpr std::uint32_t m_flPreviousValue = 0x66c;
        static constexpr std::uint32_t m_flPreviousUpdateTickTime = 0x670;
        static constexpr std::uint32_t m_vecPreviousTestPoint = 0x674;
    };

    class C_PointWorldText
    {
    public:
        static constexpr std::uint32_t m_bForceRecreateNextUpdate = 0xe90;
        static constexpr std::uint32_t m_nTextWidthPx = 0xea8;
        static constexpr std::uint32_t m_nTextHeightPx = 0xeac;
        static constexpr std::uint32_t m_messageText = 0xeb0;
        static constexpr std::uint32_t m_FontName = 0x10b0;
        static constexpr std::uint32_t m_BackgroundMaterialName = 0x10f0;
        static constexpr std::uint32_t m_bEnabled = 0x1130;
        static constexpr std::uint32_t m_bFullbright = 0x1131;
        static constexpr std::uint32_t m_flWorldUnitsPerPx = 0x1134;
        static constexpr std::uint32_t m_flFontSize = 0x1138;
        static constexpr std::uint32_t m_flDepthOffset = 0x113c;
        static constexpr std::uint32_t m_bDrawBackground = 0x1140;
        static constexpr std::uint32_t m_flBackgroundBorderWidth = 0x1144;
        static constexpr std::uint32_t m_flBackgroundBorderHeight = 0x1148;
        static constexpr std::uint32_t m_flBackgroundWorldToUV = 0x114c;
        static constexpr std::uint32_t m_Color = 0x1150;
        static constexpr std::uint32_t m_nJustifyHorizontal = 0x1154;
        static constexpr std::uint32_t m_nJustifyVertical = 0x1158;
        static constexpr std::uint32_t m_nReorientMode = 0x115c;
    };

    class C_HandleTest
    {
    public:
        static constexpr std::uint32_t m_Handle = 0x608;
        static constexpr std::uint32_t m_bSendHandle = 0x60c;
    };

    class C_EnvWind
    {
    public:
        static constexpr std::uint32_t m_EnvWindShared = 0x608;
    };

    class C_BaseToggle
    {
    public:
    };

    class C_BaseButton
    {
    public:
        static constexpr std::uint32_t m_glowEntity = 0xe88;
        static constexpr std::uint32_t m_usable = 0xe8c;
        static constexpr std::uint32_t m_szDisplayText = 0xe90;
    };

    class C_PrecipitationBlocker
    {
    public:
    };

    class C_EntityDissolve
    {
    public:
        static constexpr std::uint32_t m_flStartTime = 0xe90;
        static constexpr std::uint32_t m_flFadeInStart = 0xe94;
        static constexpr std::uint32_t m_flFadeInLength = 0xe98;
        static constexpr std::uint32_t m_flFadeOutModelStart = 0xe9c;
        static constexpr std::uint32_t m_flFadeOutModelLength = 0xea0;
        static constexpr std::uint32_t m_flFadeOutStart = 0xea4;
        static constexpr std::uint32_t m_flFadeOutLength = 0xea8;
        static constexpr std::uint32_t m_flNextSparkTime = 0xeac;
        static constexpr std::uint32_t m_nDissolveType = 0xeb0;
        static constexpr std::uint32_t m_vDissolverOrigin = 0xeb4;
        static constexpr std::uint32_t m_nMagnitude = 0xec0;
        static constexpr std::uint32_t m_bCoreExplode = 0xec4;
        static constexpr std::uint32_t m_bLinkedToServerEnt = 0xec5;
    };

    class C_EnvDecal
    {
    public:
        static constexpr std::uint32_t m_hDecalMaterial = 0xe88;
        static constexpr std::uint32_t m_flWidth = 0xe90;
        static constexpr std::uint32_t m_flHeight = 0xe94;
        static constexpr std::uint32_t m_flDepth = 0xe98;
        static constexpr std::uint32_t m_nRenderOrder = 0xe9c;
        static constexpr std::uint32_t m_bProjectOnWorld = 0xea0;
        static constexpr std::uint32_t m_bProjectOnCharacters = 0xea1;
        static constexpr std::uint32_t m_bProjectOnWater = 0xea2;
        static constexpr std::uint32_t m_flDepthSortBias = 0xea4;
    };

    class C_FuncBrush
    {
    public:
    };

    class C_FuncElectrifiedVolume
    {
    public:
        static constexpr std::uint32_t m_nAmbientEffect = 0xe88;
        static constexpr std::uint32_t m_EffectName = 0xe90;
        static constexpr std::uint32_t m_bState = 0xe98;
    };

    class C_FuncRotating
    {
    public:
    };

    class C_Breakable
    {
    public:
    };

    class C_PhysBox
    {
    public:
    };

    class C_BaseFlex
    {
    public:
        static constexpr std::uint32_t m_flexWeight = 0x1170;
        static constexpr std::uint32_t m_vLookTargetPosition = 0x1188;
        static constexpr std::uint32_t m_nLastFlexUpdateFrameCount = 0x1270;
        static constexpr std::uint32_t m_CachedViewTarget = 0x1274;
        static constexpr std::uint32_t m_nNextSceneEventId = 0x1280;
        static constexpr std::uint32_t m_iMouthAttachment = 0x1284;
        static constexpr std::uint32_t m_iEyeAttachment = 0x1285;
        static constexpr std::uint32_t m_bResetFlexWeightsOnModelChange = 0x1286;
        static constexpr std::uint32_t m_nEyeOcclusionRendererBone = 0x12a0;
        static constexpr std::uint32_t m_mEyeOcclusionRendererCameraToBoneTransform = 0x12a4;
        static constexpr std::uint32_t m_vEyeOcclusionRendererHalfExtent = 0x12d4;
        static constexpr std::uint32_t m_PhonemeClasses = 0x12f0;
    };

    class C_SceneEntity
    {
    public:
        static constexpr std::uint32_t m_bIsPlayingBack = 0x610;
        static constexpr std::uint32_t m_bPaused = 0x611;
        static constexpr std::uint32_t m_bMultiplayer = 0x612;
        static constexpr std::uint32_t m_bAutogenerated = 0x613;
        static constexpr std::uint32_t m_flForceClientTime = 0x614;
        static constexpr std::uint32_t m_nSceneStringIndex = 0x618;
        static constexpr std::uint32_t m_bClientOnly = 0x61a;
        static constexpr std::uint32_t m_hOwner = 0x61c;
        static constexpr std::uint32_t m_hActorList = 0x620;
        static constexpr std::uint32_t m_bWasPlaying = 0x638;
        static constexpr std::uint32_t m_QueuedEvents = 0x648;
        static constexpr std::uint32_t m_flCurrentTime = 0x660;
    };

    class C_TriggerVolume
    {
    public:
    };

    class C_Beam
    {
    public:
        static constexpr std::uint32_t m_flFrameRate = 0xe88;
        static constexpr std::uint32_t m_flHDRColorScale = 0xe8c;
        static constexpr std::uint32_t m_flFireTime = 0xe90;
        static constexpr std::uint32_t m_flDamage = 0xe94;
        static constexpr std::uint32_t m_nNumBeamEnts = 0xe98;
        static constexpr std::uint32_t m_queryHandleHalo = 0xe9c;
        static constexpr std::uint32_t m_hBaseMaterial = 0xec0;
        static constexpr std::uint32_t m_nHaloIndex = 0xec8;
        static constexpr std::uint32_t m_nBeamType = 0xed0;
        static constexpr std::uint32_t m_nBeamFlags = 0xed4;
        static constexpr std::uint32_t m_hAttachEntity = 0xed8;
        static constexpr std::uint32_t m_nAttachIndex = 0xf00;
        static constexpr std::uint32_t m_fWidth = 0xf0c;
        static constexpr std::uint32_t m_fEndWidth = 0xf10;
        static constexpr std::uint32_t m_fFadeLength = 0xf14;
        static constexpr std::uint32_t m_fHaloScale = 0xf18;
        static constexpr std::uint32_t m_fAmplitude = 0xf1c;
        static constexpr std::uint32_t m_fStartFrame = 0xf20;
        static constexpr std::uint32_t m_fSpeed = 0xf24;
        static constexpr std::uint32_t m_flFrame = 0xf28;
        static constexpr std::uint32_t m_nClipStyle = 0xf2c;
        static constexpr std::uint32_t m_bTurnedOff = 0xf30;
        static constexpr std::uint32_t m_vecEndPos = 0xf34;
        static constexpr std::uint32_t m_hEndEntity = 0xf40;
    };

    class C_FuncLadder
    {
    public:
        static constexpr std::uint32_t m_vecLadderDir = 0xe88;
        static constexpr std::uint32_t m_Dismounts = 0xe98;
        static constexpr std::uint32_t m_vecLocalTop = 0xeb0;
        static constexpr std::uint32_t m_vecPlayerMountPositionTop = 0xebc;
        static constexpr std::uint32_t m_vecPlayerMountPositionBottom = 0xec8;
        static constexpr std::uint32_t m_flAutoRideSpeed = 0xed4;
        static constexpr std::uint32_t m_bDisabled = 0xed8;
        static constexpr std::uint32_t m_bFakeLadder = 0xed9;
        static constexpr std::uint32_t m_bHasSlack = 0xeda;
    };

    class C_Sprite
    {
    public:
        static constexpr std::uint32_t m_hSpriteMaterial = 0xe88;
        static constexpr std::uint32_t m_hAttachedToEntity = 0xe90;
        static constexpr std::uint32_t m_nAttachment = 0xe94;
        static constexpr std::uint32_t m_flSpriteFramerate = 0xe98;
        static constexpr std::uint32_t m_flFrame = 0xe9c;
        static constexpr std::uint32_t m_flDieTime = 0xea0;
        static constexpr std::uint32_t m_nBrightness = 0xeb0;
        static constexpr std::uint32_t m_flBrightnessDuration = 0xeb4;
        static constexpr std::uint32_t m_flSpriteScale = 0xeb8;
        static constexpr std::uint32_t m_flScaleDuration = 0xebc;
        static constexpr std::uint32_t m_bWorldSpaceScale = 0xec0;
        static constexpr std::uint32_t m_flGlowProxySize = 0xec4;
        static constexpr std::uint32_t m_flHDRColorScale = 0xec8;
        static constexpr std::uint32_t m_flLastTime = 0xecc;
        static constexpr std::uint32_t m_flMaxFrame = 0xed0;
        static constexpr std::uint32_t m_flStartScale = 0xed4;
        static constexpr std::uint32_t m_flDestScale = 0xed8;
        static constexpr std::uint32_t m_flScaleTimeStart = 0xedc;
        static constexpr std::uint32_t m_nStartBrightness = 0xee0;
        static constexpr std::uint32_t m_nDestBrightness = 0xee4;
        static constexpr std::uint32_t m_flBrightnessTimeStart = 0xee8;
        static constexpr std::uint32_t m_nSpriteWidth = 0xef8;
        static constexpr std::uint32_t m_nSpriteHeight = 0xefc;
    };

    class C_BaseClientUIEntity
    {
    public:
        static constexpr std::uint32_t m_bEnabled = 0xe90;
        static constexpr std::uint32_t m_DialogXMLName = 0xe98;
        static constexpr std::uint32_t m_PanelClassName = 0xea0;
        static constexpr std::uint32_t m_PanelID = 0xea8;
    };

    class C_PointClientUIDialog
    {
    public:
        static constexpr std::uint32_t m_hActivator = 0xeb8;
        static constexpr std::uint32_t m_bStartEnabled = 0xebc;
    };

    class C_PointClientUIHUD
    {
    public:
        static constexpr std::uint32_t m_bCheckCSSClasses = 0xec0;
        static constexpr std::uint32_t m_bIgnoreInput = 0x1038;
        static constexpr std::uint32_t m_flWidth = 0x103c;
        static constexpr std::uint32_t m_flHeight = 0x1040;
        static constexpr std::uint32_t m_flDPI = 0x1044;
        static constexpr std::uint32_t m_flInteractDistance = 0x1048;
        static constexpr std::uint32_t m_flDepthOffset = 0x104c;
        static constexpr std::uint32_t m_unOwnerContext = 0x1050;
        static constexpr std::uint32_t m_unHorizontalAlign = 0x1054;
        static constexpr std::uint32_t m_unVerticalAlign = 0x1058;
        static constexpr std::uint32_t m_unOrientation = 0x105c;
        static constexpr std::uint32_t m_bAllowInteractionFromAllSceneWorlds = 0x1060;
        static constexpr std::uint32_t m_vecCSSClasses = 0x1068;
    };

    class C_PointClientUIWorldPanel
    {
    public:
        static constexpr std::uint32_t m_bForceRecreateNextUpdate = 0xec0;
        static constexpr std::uint32_t m_bMoveViewToPlayerNextThink = 0xec1;
        static constexpr std::uint32_t m_bCheckCSSClasses = 0xec2;
        static constexpr std::uint32_t m_anchorDeltaTransform = 0xed0;
        static constexpr std::uint32_t m_pOffScreenIndicator = 0x1060;
        static constexpr std::uint32_t m_bIgnoreInput = 0x1088;
        static constexpr std::uint32_t m_bLit = 0x1089;
        static constexpr std::uint32_t m_bFollowPlayerAcrossTeleport = 0x108a;
        static constexpr std::uint32_t m_flWidth = 0x108c;
        static constexpr std::uint32_t m_flHeight = 0x1090;
        static constexpr std::uint32_t m_flDPI = 0x1094;
        static constexpr std::uint32_t m_flInteractDistance = 0x1098;
        static constexpr std::uint32_t m_flDepthOffset = 0x109c;
        static constexpr std::uint32_t m_unOwnerContext = 0x10a0;
        static constexpr std::uint32_t m_unHorizontalAlign = 0x10a4;
        static constexpr std::uint32_t m_unVerticalAlign = 0x10a8;
        static constexpr std::uint32_t m_unOrientation = 0x10ac;
        static constexpr std::uint32_t m_bAllowInteractionFromAllSceneWorlds = 0x10b0;
        static constexpr std::uint32_t m_vecCSSClasses = 0x10b8;
        static constexpr std::uint32_t m_bOpaque = 0x10d0;
        static constexpr std::uint32_t m_bNoDepth = 0x10d1;
        static constexpr std::uint32_t m_bVisibleWhenParentNoDraw = 0x10d2;
        static constexpr std::uint32_t m_bRenderBackface = 0x10d3;
        static constexpr std::uint32_t m_bUseOffScreenIndicator = 0x10d4;
        static constexpr std::uint32_t m_bExcludeFromSaveGames = 0x10d5;
        static constexpr std::uint32_t m_bGrabbable = 0x10d6;
        static constexpr std::uint32_t m_bOnlyRenderToTexture = 0x10d7;
        static constexpr std::uint32_t m_bDisableMipGen = 0x10d8;
        static constexpr std::uint32_t m_nExplicitImageLayout = 0x10dc;
    };

    class C_PointClientUIWorldTextPanel
    {
    public:
        static constexpr std::uint32_t m_messageText = 0x10e0;
    };

    class C_InfoInstructorHintHostageRescueZone
    {
    public:
    };

    class C_CSObserverPawn
    {
    public:
        static constexpr std::uint32_t m_hDetectParentChange = 0x1650;
    };

    class C_PlayerSprayDecal
    {
    public:
        static constexpr std::uint32_t m_nUniqueID = 0xe88;
        static constexpr std::uint32_t m_unAccountID = 0xe8c;
        static constexpr std::uint32_t m_unTraceID = 0xe90;
        static constexpr std::uint32_t m_rtGcTime = 0xe94;
        static constexpr std::uint32_t m_vecEndPos = 0xe98;
        static constexpr std::uint32_t m_vecStart = 0xea4;
        static constexpr std::uint32_t m_vecLeft = 0xeb0;
        static constexpr std::uint32_t m_vecNormal = 0xebc;
        static constexpr std::uint32_t m_nPlayer = 0xec8;
        static constexpr std::uint32_t m_nEntity = 0xecc;
        static constexpr std::uint32_t m_nHitbox = 0xed0;
        static constexpr std::uint32_t m_flCreationTime = 0xed4;
        static constexpr std::uint32_t m_nTintID = 0xed8;
        static constexpr std::uint32_t m_nVersion = 0xedc;
        static constexpr std::uint32_t m_ubSignature = 0xedd;
        static constexpr std::uint32_t m_SprayRenderHelper = 0xf68;
    };

    class C_FuncConveyor
    {
    public:
        static constexpr std::uint32_t m_vecMoveDirEntitySpace = 0xe90;
        static constexpr std::uint32_t m_flTargetSpeed = 0xe9c;
        static constexpr std::uint32_t m_nTransitionStartTick = 0xea0;
        static constexpr std::uint32_t m_nTransitionDurationTicks = 0xea4;
        static constexpr std::uint32_t m_flTransitionStartSpeed = 0xea8;
        static constexpr std::uint32_t m_hConveyorModels = 0xeb0;
        static constexpr std::uint32_t m_flCurrentConveyorOffset = 0xec8;
        static constexpr std::uint32_t m_flCurrentConveyorSpeed = 0xecc;
    };

    class C_Inferno
    {
    public:
        static constexpr std::uint32_t m_nfxFireDamageEffect = 0xec8;
        static constexpr std::uint32_t m_hInfernoPointsSnapshot = 0xed0;
        static constexpr std::uint32_t m_hInfernoFillerPointsSnapshot = 0xed8;
        static constexpr std::uint32_t m_hInfernoOutlinePointsSnapshot = 0xee0;
        static constexpr std::uint32_t m_hInfernoClimbingOutlinePointsSnapshot = 0xee8;
        static constexpr std::uint32_t m_hInfernoDecalsSnapshot = 0xef0;
        static constexpr std::uint32_t m_firePositions = 0xef8;
        static constexpr std::uint32_t m_fireParentPositions = 0x11f8;
        static constexpr std::uint32_t m_bFireIsBurning = 0x14f8;
        static constexpr std::uint32_t m_BurnNormal = 0x1538;
        static constexpr std::uint32_t m_fireCount = 0x1838;
        static constexpr std::uint32_t m_nInfernoType = 0x183c;
        static constexpr std::uint32_t m_nFireLifetime = 0x1840;
        static constexpr std::uint32_t m_bInPostEffectTime = 0x1844;
        static constexpr std::uint32_t m_lastFireCount = 0x1848;
        static constexpr std::uint32_t m_nFireEffectTickBegin = 0x184c;
        static constexpr std::uint32_t m_drawableCount = 0x8450;
        static constexpr std::uint32_t m_blosCheck = 0x8454;
        static constexpr std::uint32_t m_nlosperiod = 0x8458;
        static constexpr std::uint32_t m_maxFireHalfWidth = 0x845c;
        static constexpr std::uint32_t m_maxFireHeight = 0x8460;
        static constexpr std::uint32_t m_minBounds = 0x8464;
        static constexpr std::uint32_t m_maxBounds = 0x8470;
        static constexpr std::uint32_t m_flLastGrassBurnThink = 0x847c;
    };

    class C_FireCrackerBlast
    {
    public:
    };

    class C_BarnLight
    {
    public:
        static constexpr std::uint32_t m_bEnabled = 0xe88;
        static constexpr std::uint32_t m_nColorMode = 0xe8c;
        static constexpr std::uint32_t m_Color = 0xe90;
        static constexpr std::uint32_t m_flColorTemperature = 0xe94;
        static constexpr std::uint32_t m_flBrightness = 0xe98;
        static constexpr std::uint32_t m_flBrightnessScale = 0xe9c;
        static constexpr std::uint32_t m_nDirectLight = 0xea0;
        static constexpr std::uint32_t m_nBakedShadowIndex = 0xea4;
        static constexpr std::uint32_t m_nLightPathUniqueId = 0xea8;
        static constexpr std::uint32_t m_nLightMapUniqueId = 0xeac;
        static constexpr std::uint32_t m_nLuminaireShape = 0xeb0;
        static constexpr std::uint32_t m_flLuminaireSize = 0xeb4;
        static constexpr std::uint32_t m_flLuminaireAnisotropy = 0xeb8;
        static constexpr std::uint32_t m_LightStyleString = 0xec0;
        static constexpr std::uint32_t m_flLightStyleStartTime = 0xec8;
        static constexpr std::uint32_t m_QueuedLightStyleStrings = 0xed0;
        static constexpr std::uint32_t m_LightStyleEvents = 0xee8;
        static constexpr std::uint32_t m_LightStyleTargets = 0xf00;
        static constexpr std::uint32_t m_StyleEvent = 0xf18;
        static constexpr std::uint32_t m_hLightCookie = 0xf78;
        static constexpr std::uint32_t m_flShape = 0xf80;
        static constexpr std::uint32_t m_flSoftX = 0xf84;
        static constexpr std::uint32_t m_flSoftY = 0xf88;
        static constexpr std::uint32_t m_flSkirt = 0xf8c;
        static constexpr std::uint32_t m_flSkirtNear = 0xf90;
        static constexpr std::uint32_t m_vSizeParams = 0xf94;
        static constexpr std::uint32_t m_flRange = 0xfa0;
        static constexpr std::uint32_t m_vShear = 0xfa4;
        static constexpr std::uint32_t m_nBakeSpecularToCubemaps = 0xfb0;
        static constexpr std::uint32_t m_vBakeSpecularToCubemapsSize = 0xfb4;
        static constexpr std::uint32_t m_nCastShadows = 0xfc0;
        static constexpr std::uint32_t m_nShadowMapSize = 0xfc4;
        static constexpr std::uint32_t m_nShadowPriority = 0xfc8;
        static constexpr std::uint32_t m_bContactShadow = 0xfcc;
        static constexpr std::uint32_t m_bForceShadowsEnabled = 0xfcd;
        static constexpr std::uint32_t m_nBounceLight = 0xfd0;
        static constexpr std::uint32_t m_flBounceScale = 0xfd4;
        static constexpr std::uint32_t m_bDynamicBounce = 0xfd8;
        static constexpr std::uint32_t m_flMinRoughness = 0xfdc;
        static constexpr std::uint32_t m_vAlternateColor = 0xfe0;
        static constexpr std::uint32_t m_fAlternateColorBrightness = 0xfec;
        static constexpr std::uint32_t m_nFog = 0xff0;
        static constexpr std::uint32_t m_flFogStrength = 0xff4;
        static constexpr std::uint32_t m_nFogShadows = 0xff8;
        static constexpr std::uint32_t m_flFogScale = 0xffc;
        static constexpr std::uint32_t m_bFogMixedShadows = 0x1000;
        static constexpr std::uint32_t m_flFadeSizeStart = 0x1004;
        static constexpr std::uint32_t m_flFadeSizeEnd = 0x1008;
        static constexpr std::uint32_t m_flShadowFadeSizeStart = 0x100c;
        static constexpr std::uint32_t m_flShadowFadeSizeEnd = 0x1010;
        static constexpr std::uint32_t m_bPrecomputedFieldsValid = 0x1014;
        static constexpr std::uint32_t m_vPrecomputedBoundsMins = 0x1018;
        static constexpr std::uint32_t m_vPrecomputedBoundsMaxs = 0x1024;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin = 0x1030;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles = 0x103c;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent = 0x1048;
        static constexpr std::uint32_t m_nPrecomputedSubFrusta = 0x1054;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin0 = 0x1058;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles0 = 0x1064;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent0 = 0x1070;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin1 = 0x107c;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles1 = 0x1088;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent1 = 0x1094;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin2 = 0x10a0;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles2 = 0x10ac;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent2 = 0x10b8;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin3 = 0x10c4;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles3 = 0x10d0;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent3 = 0x10dc;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin4 = 0x10e8;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles4 = 0x10f4;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent4 = 0x1100;
        static constexpr std::uint32_t m_vPrecomputedOBBOrigin5 = 0x110c;
        static constexpr std::uint32_t m_vPrecomputedOBBAngles5 = 0x1118;
        static constexpr std::uint32_t m_vPrecomputedOBBExtent5 = 0x1124;
        static constexpr std::uint32_t m_bInitialBoneSetup = 0x1170;
        static constexpr std::uint32_t m_VisClusters = 0x1178;
    };

    class C_RectLight
    {
    public:
        static constexpr std::uint32_t m_bShowLight = 0x1198;
    };

    class C_OmniLight
    {
    public:
        static constexpr std::uint32_t m_flInnerAngle = 0x1198;
        static constexpr std::uint32_t m_flOuterAngle = 0x119c;
        static constexpr std::uint32_t m_bShowLight = 0x11a0;
    };

    class C_CSTeam
    {
    public:
        static constexpr std::uint32_t m_szTeamMatchStat = 0x6c0;
        static constexpr std::uint32_t m_numMapVictories = 0x8c0;
        static constexpr std::uint32_t m_bSurrendered = 0x8c4;
        static constexpr std::uint32_t m_scoreFirstHalf = 0x8c8;
        static constexpr std::uint32_t m_scoreSecondHalf = 0x8cc;
        static constexpr std::uint32_t m_scoreOvertime = 0x8d0;
        static constexpr std::uint32_t m_szClanTeamname = 0x8d4;
        static constexpr std::uint32_t m_iClanID = 0x958;
        static constexpr std::uint32_t m_szTeamFlagImage = 0x95c;
        static constexpr std::uint32_t m_szTeamLogoImage = 0x964;
    };

    class C_EnvSky
    {
    public:
        static constexpr std::uint32_t m_hSkyMaterial = 0xe88;
        static constexpr std::uint32_t m_hSkyMaterialLightingOnly = 0xe90;
        static constexpr std::uint32_t m_bStartDisabled = 0xe98;
        static constexpr std::uint32_t m_vTintColor = 0xe99;
        static constexpr std::uint32_t m_vTintColorLightingOnly = 0xe9d;
        static constexpr std::uint32_t m_flBrightnessScale = 0xea4;
        static constexpr std::uint32_t m_nFogType = 0xea8;
        static constexpr std::uint32_t m_flFogMinStart = 0xeac;
        static constexpr std::uint32_t m_flFogMinEnd = 0xeb0;
        static constexpr std::uint32_t m_flFogMaxStart = 0xeb4;
        static constexpr std::uint32_t m_flFogMaxEnd = 0xeb8;
        static constexpr std::uint32_t m_bEnabled = 0xebc;
    };

    class C_TonemapController2Alias_env_tonemap_controller2
    {
    public:
    };

    class C_LightEntity
    {
    public:
        static constexpr std::uint32_t m_CLightComponent = 0xe88;
    };

    class C_LightSpotEntity
    {
    public:
    };

    class C_LightOrthoEntity
    {
    public:
    };

    class C_LightDirectionalEntity
    {
    public:
    };

    class C_LightEnvironmentEntity
    {
    public:
    };

    class C_BaseTrigger
    {
    public:
        static constexpr std::uint32_t m_OnStartTouch = 0xe88;
        static constexpr std::uint32_t m_OnStartTouchAll = 0xea0;
        static constexpr std::uint32_t m_OnEndTouch = 0xeb8;
        static constexpr std::uint32_t m_OnEndTouchAll = 0xed0;
        static constexpr std::uint32_t m_OnTouching = 0xee8;
        static constexpr std::uint32_t m_OnTouchingEachEntity = 0xf00;
        static constexpr std::uint32_t m_OnNotTouching = 0xf18;
        static constexpr std::uint32_t m_hTouchingEntities = 0xf30;
        static constexpr std::uint32_t m_iFilterName = 0xf48;
        static constexpr std::uint32_t m_hFilter = 0xf50;
        static constexpr std::uint32_t m_bDisabled = 0xf54;
    };

    class C_ParticleSystem
    {
    public:
        static constexpr std::uint32_t m_szSnapshotFileName = 0xe88;
        static constexpr std::uint32_t m_bActive = 0x1088;
        static constexpr std::uint32_t m_bFrozen = 0x1089;
        static constexpr std::uint32_t m_flFreezeTransitionDuration = 0x108c;
        static constexpr std::uint32_t m_nStopType = 0x1090;
        static constexpr std::uint32_t m_bAnimateDuringGameplayPause = 0x1094;
        static constexpr std::uint32_t m_iEffectIndex = 0x1098;
        static constexpr std::uint32_t m_flStartTime = 0x10a0;
        static constexpr std::uint32_t m_flPreSimTime = 0x10a4;
        static constexpr std::uint32_t m_vServerControlPoints = 0x10a8;
        static constexpr std::uint32_t m_iServerControlPointAssignments = 0x10d8;
        static constexpr std::uint32_t m_hControlPointEnts = 0x10dc;
        static constexpr std::uint32_t m_bNoSave = 0x11dc;
        static constexpr std::uint32_t m_bNoFreeze = 0x11dd;
        static constexpr std::uint32_t m_bNoRamp = 0x11de;
        static constexpr std::uint32_t m_bStartActive = 0x11df;
        static constexpr std::uint32_t m_iszEffectName = 0x11e0;
        static constexpr std::uint32_t m_iszControlPointNames = 0x11e8;
        static constexpr std::uint32_t m_nDataCP = 0x13e8;
        static constexpr std::uint32_t m_vecDataCPValue = 0x13ec;
        static constexpr std::uint32_t m_nTintCP = 0x13f8;
        static constexpr std::uint32_t m_clrTint = 0x13fc;
        static constexpr std::uint32_t m_bOldActive = 0x1420;
        static constexpr std::uint32_t m_bOldFrozen = 0x1421;
    };

    class C_TextureBasedAnimatable
    {
    public:
        static constexpr std::uint32_t m_bLoop = 0xe88;
        static constexpr std::uint32_t m_flFPS = 0xe8c;
        static constexpr std::uint32_t m_hPositionKeys = 0xe90;
        static constexpr std::uint32_t m_hRotationKeys = 0xe98;
        static constexpr std::uint32_t m_vAnimationBoundsMin = 0xea0;
        static constexpr std::uint32_t m_vAnimationBoundsMax = 0xeac;
        static constexpr std::uint32_t m_flStartTime = 0xeb8;
        static constexpr std::uint32_t m_flStartFrame = 0xebc;
    };

    class C_World
    {
    public:
    };

    class C_BreakableProp
    {
    public:
        static constexpr std::uint32_t m_CPropDataComponent = 0x11a0;
        static constexpr std::uint32_t m_OnStartDeath = 0x11e0;
        static constexpr std::uint32_t m_OnBreak = 0x11f8;
        static constexpr std::uint32_t m_OnHealthChanged = 0x1210;
        static constexpr std::uint32_t m_OnTakeDamage = 0x1230;
        static constexpr std::uint32_t m_impactEnergyScale = 0x1248;
        static constexpr std::uint32_t m_iMinHealthDmg = 0x124c;
        static constexpr std::uint32_t m_flPressureDelay = 0x1250;
        static constexpr std::uint32_t m_flDefBurstScale = 0x1254;
        static constexpr std::uint32_t m_vDefBurstOffset = 0x1258;
        static constexpr std::uint32_t m_hBreaker = 0x1264;
        static constexpr std::uint32_t m_PerformanceMode = 0x1268;
        static constexpr std::uint32_t m_flPreventDamageBeforeTime = 0x126c;
        static constexpr std::uint32_t m_BreakableContentsType = 0x1270;
        static constexpr std::uint32_t m_strBreakableContentsPropGroupOverride = 0x1278;
        static constexpr std::uint32_t m_strBreakableContentsParticleOverride = 0x1280;
        static constexpr std::uint32_t m_bHasBreakPiecesOrCommands = 0x1288;
        static constexpr std::uint32_t m_explodeDamage = 0x128c;
        static constexpr std::uint32_t m_explodeRadius = 0x1290;
        static constexpr std::uint32_t m_nExplosionType = 0x1294;
        static constexpr std::uint32_t m_explosionDelay = 0x1298;
        static constexpr std::uint32_t m_explosionBuildupSound = 0x12a0;
        static constexpr std::uint32_t m_explosionCustomEffect = 0x12a8;
        static constexpr std::uint32_t m_explosionCustomSound = 0x12b0;
        static constexpr std::uint32_t m_explosionModifier = 0x12b8;
        static constexpr std::uint32_t m_hPhysicsAttacker = 0x12c0;
        static constexpr std::uint32_t m_flLastPhysicsInfluenceTime = 0x12c4;
        static constexpr std::uint32_t m_flDefaultFadeScale = 0x12c8;
        static constexpr std::uint32_t m_hLastAttacker = 0x12cc;
    };

    class C_DynamicProp
    {
    public:
        static constexpr std::uint32_t m_bUseHitboxesForRenderBox = 0x12d0;
        static constexpr std::uint32_t m_bUseAnimGraph = 0x12d1;
        static constexpr std::uint32_t m_pOutputAnimBegun = 0x12d8;
        static constexpr std::uint32_t m_pOutputAnimOver = 0x12f0;
        static constexpr std::uint32_t m_pOutputAnimLoopCycleOver = 0x1308;
        static constexpr std::uint32_t m_OnAnimReachedStart = 0x1320;
        static constexpr std::uint32_t m_OnAnimReachedEnd = 0x1338;
        static constexpr std::uint32_t m_iszIdleAnim = 0x1350;
        static constexpr std::uint32_t m_nIdleAnimLoopMode = 0x1358;
        static constexpr std::uint32_t m_bRandomizeCycle = 0x135c;
        static constexpr std::uint32_t m_bStartDisabled = 0x135d;
        static constexpr std::uint32_t m_bFiredStartEndOutput = 0x135e;
        static constexpr std::uint32_t m_bForceNpcExclude = 0x135f;
        static constexpr std::uint32_t m_bCreateNonSolid = 0x1360;
        static constexpr std::uint32_t m_bIsOverrideProp = 0x1361;
        static constexpr std::uint32_t m_iInitialGlowState = 0x1364;
        static constexpr std::uint32_t m_nGlowRange = 0x1368;
        static constexpr std::uint32_t m_nGlowRangeMin = 0x136c;
        static constexpr std::uint32_t m_glowColor = 0x1370;
        static constexpr std::uint32_t m_nGlowTeam = 0x1374;
        static constexpr std::uint32_t m_iCachedFrameCount = 0x1378;
        static constexpr std::uint32_t m_vecCachedRenderMins = 0x137c;
        static constexpr std::uint32_t m_vecCachedRenderMaxs = 0x1388;
    };

    class C_DynamicPropAlias_dynamic_prop
    {
    public:
    };

    class C_DynamicPropAlias_prop_dynamic_override
    {
    public:
    };

    class C_DynamicPropAlias_cable_dynamic
    {
    public:
    };

    class C_ColorCorrectionVolume
    {
    public:
        static constexpr std::uint32_t m_LastEnterWeight = 0xf58;
        static constexpr std::uint32_t m_LastEnterTime = 0xf5c;
        static constexpr std::uint32_t m_LastExitWeight = 0xf60;
        static constexpr std::uint32_t m_LastExitTime = 0xf64;
        static constexpr std::uint32_t m_bEnabled = 0xf68;
        static constexpr std::uint32_t m_MaxWeight = 0xf6c;
        static constexpr std::uint32_t m_FadeDuration = 0xf70;
        static constexpr std::uint32_t m_Weight = 0xf74;
        static constexpr std::uint32_t m_lookupFilename = 0xf78;
    };

    class C_FuncMonitor
    {
    public:
        static constexpr std::uint32_t m_targetCamera = 0xe88;
        static constexpr std::uint32_t m_nResolutionEnum = 0xe90;
        static constexpr std::uint32_t m_bRenderShadows = 0xe94;
        static constexpr std::uint32_t m_bUseUniqueColorTarget = 0xe95;
        static constexpr std::uint32_t m_brushModelName = 0xe98;
        static constexpr std::uint32_t m_hTargetCamera = 0xea0;
        static constexpr std::uint32_t m_bEnabled = 0xea4;
        static constexpr std::uint32_t m_bDraw3DSkybox = 0xea5;
    };

    class C_FuncMoveLinear
    {
    public:
    };

    class C_FuncMover
    {
    public:
    };

    class C_PhysMagnet
    {
    public:
        static constexpr std::uint32_t m_aAttachedObjectsFromServer = 0x1168;
        static constexpr std::uint32_t m_aAttachedObjects = 0x1180;
    };

    class C_PointCommentaryNode
    {
    public:
        static constexpr std::uint32_t m_bActive = 0x1180;
        static constexpr std::uint32_t m_bWasActive = 0x1181;
        static constexpr std::uint32_t m_flEndTime = 0x1184;
        static constexpr std::uint32_t m_flStartTime = 0x1188;
        static constexpr std::uint32_t m_flStartTimeInCommentary = 0x118c;
        static constexpr std::uint32_t m_iszCommentaryFile = 0x1190;
        static constexpr std::uint32_t m_iszTitle = 0x1198;
        static constexpr std::uint32_t m_iszSpeakers = 0x11a0;
        static constexpr std::uint32_t m_iNodeNumber = 0x11a8;
        static constexpr std::uint32_t m_iNodeNumberMax = 0x11ac;
        static constexpr std::uint32_t m_bListenedTo = 0x11b0;
        static constexpr std::uint32_t m_sndCommentary = 0x11b8;
        static constexpr std::uint32_t m_hViewPosition = 0x11c0;
        static constexpr std::uint32_t m_bRestartAfterRestore = 0x11c4;
    };

    class C_WaterBullet
    {
    public:
    };

    class C_BaseDoor
    {
    public:
        static constexpr std::uint32_t m_bIsUsable = 0xe88;
    };

    class C_ClientRagdoll
    {
    public:
        static constexpr std::uint32_t m_bFadeOut = 0x1168;
        static constexpr std::uint32_t m_bImportant = 0x1169;
        static constexpr std::uint32_t m_flEffectTime = 0x116c;
        static constexpr std::uint32_t m_gibDespawnTime = 0x1170;
        static constexpr std::uint32_t m_iCurrentFriction = 0x1174;
        static constexpr std::uint32_t m_iMinFriction = 0x1178;
        static constexpr std::uint32_t m_iMaxFriction = 0x117c;
        static constexpr std::uint32_t m_iFrictionAnimState = 0x1180;
        static constexpr std::uint32_t m_bReleaseRagdoll = 0x1184;
        static constexpr std::uint32_t m_iEyeAttachment = 0x1185;
        static constexpr std::uint32_t m_bFadingOut = 0x1186;
        static constexpr std::uint32_t m_flScaleEnd = 0x1188;
        static constexpr std::uint32_t m_flScaleTimeStart = 0x11b0;
        static constexpr std::uint32_t m_flScaleTimeEnd = 0x11d8;
    };

    class C_Precipitation
    {
    public:
        static constexpr std::uint32_t m_flDensity = 0xf58;
        static constexpr std::uint32_t m_flParticleInnerDist = 0xf68;
        static constexpr std::uint32_t m_pParticleDef = 0xf70;
        static constexpr std::uint32_t m_tParticlePrecipTraceTimer = 0xf98;
        static constexpr std::uint32_t m_bActiveParticlePrecipEmitter = 0xfa0;
        static constexpr std::uint32_t m_bParticlePrecipInitialized = 0xfa1;
        static constexpr std::uint32_t m_bHasSimulatedSinceLastSceneObjectUpdate = 0xfa2;
        static constexpr std::uint32_t m_nAvailableSheetSequencesMaxIndex = 0xfa4;
    };

    class C_Fish
    {
    public:
        static constexpr std::uint32_t m_pos = 0x1168;
        static constexpr std::uint32_t m_vel = 0x1174;
        static constexpr std::uint32_t m_angles = 0x1180;
        static constexpr std::uint32_t m_localLifeState = 0x118c;
        static constexpr std::uint32_t m_deathDepth = 0x1190;
        static constexpr std::uint32_t m_deathAngle = 0x1194;
        static constexpr std::uint32_t m_buoyancy = 0x1198;
        static constexpr std::uint32_t m_wiggleTimer = 0x11a0;
        static constexpr std::uint32_t m_wigglePhase = 0x11b8;
        static constexpr std::uint32_t m_wiggleRate = 0x11bc;
        static constexpr std::uint32_t m_actualPos = 0x11c0;
        static constexpr std::uint32_t m_actualAngles = 0x11cc;
        static constexpr std::uint32_t m_poolOrigin = 0x11d8;
        static constexpr std::uint32_t m_waterLevel = 0x11e4;
        static constexpr std::uint32_t m_gotUpdate = 0x11e8;
        static constexpr std::uint32_t m_x = 0x11ec;
        static constexpr std::uint32_t m_y = 0x11f0;
        static constexpr std::uint32_t m_z = 0x11f4;
        static constexpr std::uint32_t m_angle = 0x11f8;
        static constexpr std::uint32_t m_errorHistory = 0x11fc;
        static constexpr std::uint32_t m_errorHistoryIndex = 0x124c;
        static constexpr std::uint32_t m_errorHistoryCount = 0x1250;
        static constexpr std::uint32_t m_averageError = 0x1254;
    };

    class C_PhysicsProp
    {
    public:
        static constexpr std::uint32_t m_bAwake = 0x12d0;
    };

    class C_BasePropDoor
    {
    public:
        static constexpr std::uint32_t m_eDoorState = 0x13b0;
        static constexpr std::uint32_t m_modelChanged = 0x13b4;
        static constexpr std::uint32_t m_bLocked = 0x13b5;
        static constexpr std::uint32_t m_bNoNPCs = 0x13b6;
        static constexpr std::uint32_t m_closedPosition = 0x13b8;
        static constexpr std::uint32_t m_closedAngles = 0x13c4;
        static constexpr std::uint32_t m_hMaster = 0x13d0;
        static constexpr std::uint32_t m_vWhereToSetLightingOrigin = 0x13d4;
    };

    class C_PropDoorRotating
    {
    public:
    };

    class C_TriggerMultiple
    {
    public:
    };

    class C_TriggerLerpObject
    {
    public:
    };

    class C_TriggerPhysics
    {
    public:
        static constexpr std::uint32_t m_gravityScale = 0xf58;
        static constexpr std::uint32_t m_linearLimit = 0xf5c;
        static constexpr std::uint32_t m_linearDamping = 0xf60;
        static constexpr std::uint32_t m_angularLimit = 0xf64;
        static constexpr std::uint32_t m_angularDamping = 0xf68;
        static constexpr std::uint32_t m_linearForce = 0xf6c;
        static constexpr std::uint32_t m_flFrequency = 0xf70;
        static constexpr std::uint32_t m_flDampingRatio = 0xf74;
        static constexpr std::uint32_t m_vecLinearForcePointAt = 0xf78;
        static constexpr std::uint32_t m_bCollapseToForcePoint = 0xf84;
        static constexpr std::uint32_t m_vecLinearForcePointAtWorld = 0xf88;
        static constexpr std::uint32_t m_vecLinearForceDirection = 0xf94;
        static constexpr std::uint32_t m_bConvertToDebrisWhenPossible = 0xfa0;
    };

    class C_PhysPropClientside
    {
    public:
        static constexpr std::uint32_t m_flTouchDelta = 0x12d0;
        static constexpr std::uint32_t m_fDeathTime = 0x12d4;
        static constexpr std::uint32_t m_vecDamagePosition = 0x12d8;
        static constexpr std::uint32_t m_vecDamageDirection = 0x12e4;
        static constexpr std::uint32_t m_nDamageType = 0x12f0;
    };

    class C_RagdollProp
    {
    public:
        static constexpr std::uint32_t m_ragEnabled = 0x1170;
        static constexpr std::uint32_t m_ragPos = 0x1188;
        static constexpr std::uint32_t m_ragAngles = 0x11a0;
        static constexpr std::uint32_t m_flBlendWeight = 0x11b8;
        static constexpr std::uint32_t m_hRagdollSource = 0x11bc;
        static constexpr std::uint32_t m_iEyeAttachment = 0x11c0;
        static constexpr std::uint32_t m_flBlendWeightCurrent = 0x11c4;
        static constexpr std::uint32_t m_parentPhysicsBoneIndices = 0x11c8;
        static constexpr std::uint32_t m_worldSpaceBoneComputationOrder = 0x11e0;
    };

    class C_LocalTempEntity
    {
    public:
        static constexpr std::uint32_t flags = 0x1168;
        static constexpr std::uint32_t die = 0x116c;
        static constexpr std::uint32_t m_flFrameMax = 0x1170;
        static constexpr std::uint32_t x = 0x1174;
        static constexpr std::uint32_t y = 0x1178;
        static constexpr std::uint32_t fadeSpeed = 0x117c;
        static constexpr std::uint32_t bounceFactor = 0x1180;
        static constexpr std::uint32_t hitSound = 0x1184;
        static constexpr std::uint32_t priority = 0x1188;
        static constexpr std::uint32_t tentOffset = 0x118c;
        static constexpr std::uint32_t m_vecTempEntAngVelocity = 0x1198;
        static constexpr std::uint32_t tempent_renderamt = 0x11a4;
        static constexpr std::uint32_t m_vecNormal = 0x11a8;
        static constexpr std::uint32_t m_flSpriteScale = 0x11b4;
        static constexpr std::uint32_t m_nFlickerFrame = 0x11b8;
        static constexpr std::uint32_t m_flFrameRate = 0x11bc;
        static constexpr std::uint32_t m_flFrame = 0x11c0;
        static constexpr std::uint32_t m_pszImpactEffect = 0x11c8;
        static constexpr std::uint32_t m_pszParticleEffect = 0x11d0;
        static constexpr std::uint32_t m_bParticleCollision = 0x11d8;
        static constexpr std::uint32_t m_iLastCollisionFrame = 0x11dc;
        static constexpr std::uint32_t m_vLastCollisionOrigin = 0x11e0;
        static constexpr std::uint32_t m_vecTempEntVelocity = 0x11ec;
        static constexpr std::uint32_t m_vecPrevAbsOrigin = 0x11f8;
        static constexpr std::uint32_t m_vecTempEntAcceleration = 0x1204;
    };

    class C_ShatterGlassShardPhysics
    {
    public:
        static constexpr std::uint32_t m_ShardDesc = 0x12e8;
    };

    class C_EconEntity
    {
    public:
        static constexpr std::uint32_t m_flFlexDelayTime = 0x1360;
        static constexpr std::uint32_t m_flFlexDelayedWeight = 0x1368;
        static constexpr std::uint32_t m_bAttributesInitialized = 0x1370;
        static constexpr std::uint32_t m_AttributeManager = 0x1378;
        static constexpr std::uint32_t m_OriginalOwnerXuidLow = 0x1848;
        static constexpr std::uint32_t m_OriginalOwnerXuidHigh = 0x184c;
        static constexpr std::uint32_t m_nFallbackPaintKit = 0x1850;
        static constexpr std::uint32_t m_nFallbackSeed = 0x1854;
        static constexpr std::uint32_t m_flFallbackWear = 0x1858;
        static constexpr std::uint32_t m_nFallbackStatTrak = 0x185c;
        static constexpr std::uint32_t m_bClientside = 0x1860;
        static constexpr std::uint32_t m_bParticleSystemsCreated = 0x1861;
        static constexpr std::uint32_t m_vecAttachedParticles = 0x1868;
        static constexpr std::uint32_t m_hViewmodelAttachment = 0x1880;
        static constexpr std::uint32_t m_iOldTeam = 0x1884;
        static constexpr std::uint32_t m_bAttachmentDirty = 0x1888;
        static constexpr std::uint32_t m_nUnloadedModelIndex = 0x188c;
        static constexpr std::uint32_t m_iNumOwnerValidationRetries = 0x1890;
        static constexpr std::uint32_t m_hOldProvidee = 0x18a0;
        static constexpr std::uint32_t m_vecAttachedModels = 0x18a8;
    };

    class C_EconWearable
    {
    public:
        static constexpr std::uint32_t m_nForceSkin = 0x18c0;
        static constexpr std::uint32_t m_bAlwaysAllow = 0x18c4;
    };

    class C_BaseGrenade
    {
    public:
        static constexpr std::uint32_t m_bHasWarnedAI = 0x1350;
        static constexpr std::uint32_t m_bIsSmokeGrenade = 0x1351;
        static constexpr std::uint32_t m_bIsLive = 0x1352;
        static constexpr std::uint32_t m_DmgRadius = 0x1354;
        static constexpr std::uint32_t m_flDetonateTime = 0x1358;
        static constexpr std::uint32_t m_flWarnAITime = 0x135c;
        static constexpr std::uint32_t m_flDamage = 0x1360;
        static constexpr std::uint32_t m_iszBounceSound = 0x1368;
        static constexpr std::uint32_t m_ExplosionSound = 0x1370;
        static constexpr std::uint32_t m_hThrower = 0x137c;
        static constexpr std::uint32_t m_flNextAttack = 0x1394;
        static constexpr std::uint32_t m_hOriginalThrower = 0x1398;
    };

    class C_TriggerBuoyancy
    {
    public:
        static constexpr std::uint32_t m_BuoyancyHelper = 0xf58;
        static constexpr std::uint32_t m_flFluidDensity = 0x1070;
    };

    class C_PhysicsPropMultiplayer
    {
    public:
    };

    class C_FootstepControl
    {
    public:
        static constexpr std::uint32_t m_source = 0xf58;
        static constexpr std::uint32_t m_destination = 0xf60;
    };

    class C_CS2WeaponModuleBase
    {
    public:
    };

    class C_StattrakModule
    {
    public:
        static constexpr std::uint32_t m_bKnife = 0x1170;
    };

    class C_NametagModule
    {
    public:
        static constexpr std::uint32_t m_strNametagString = 0x1170;
    };

    class C_KeychainModule
    {
    public:
        static constexpr std::uint32_t m_nKeychainDefID = 0x1170;
        static constexpr std::uint32_t m_nKeychainSeed = 0x1174;
    };

    class C_BaseCSGrenadeProjectile
    {
    public:
        static constexpr std::uint32_t m_vInitialPosition = 0x13a0;
        static constexpr std::uint32_t m_vInitialVelocity = 0x13ac;
        static constexpr std::uint32_t m_nBounces = 0x13b8;
        static constexpr std::uint32_t m_nExplodeEffectIndex = 0x13c0;
        static constexpr std::uint32_t m_nExplodeEffectTickBegin = 0x13c8;
        static constexpr std::uint32_t m_vecExplodeEffectOrigin = 0x13cc;
        static constexpr std::uint32_t m_flSpawnTime = 0x13d8;
        static constexpr std::uint32_t vecLastTrailLinePos = 0x13dc;
        static constexpr std::uint32_t flNextTrailLineTime = 0x13e8;
        static constexpr std::uint32_t m_bExplodeEffectBegan = 0x13ec;
        static constexpr std::uint32_t m_bCanCreateGrenadeTrail = 0x13ed;
        static constexpr std::uint32_t m_nSnapshotTrajectoryEffectIndex = 0x13f0;
        static constexpr std::uint32_t m_hSnapshotTrajectoryParticleSnapshot = 0x13f8;
        static constexpr std::uint32_t m_arrTrajectoryTrailPoints = 0x1400;
        static constexpr std::uint32_t m_arrTrajectoryTrailPointCreationTimes = 0x1418;
        static constexpr std::uint32_t m_flTrajectoryTrailEffectCreationTime = 0x1430;
    };

    class C_CSGO_PreviewModel
    {
    public:
        static constexpr std::uint32_t m_defaultAnim = 0x1350;
        static constexpr std::uint32_t m_nDefaultAnimLoopMode = 0x1358;
        static constexpr std::uint32_t m_flInitialModelScale = 0x135c;
        static constexpr std::uint32_t m_sInitialWeaponState = 0x1360;
    };

    class C_CSGO_PreviewModelAlias_csgo_item_previewmodel
    {
    public:
    };

    class C_WorldModelGloves
    {
    public:
    };

    class C_BulletHitModel
    {
    public:
        static constexpr std::uint32_t m_matLocal = 0x1168;
        static constexpr std::uint32_t m_iBoneIndex = 0x1198;
        static constexpr std::uint32_t m_hPlayerParent = 0x119c;
        static constexpr std::uint32_t m_bIsHit = 0x11a0;
        static constexpr std::uint32_t m_flTimeCreated = 0x11a4;
        static constexpr std::uint32_t m_vecStartPos = 0x11a8;
    };

    class C_HostageCarriableProp
    {
    public:
    };

    class C_PlantedC4
    {
    public:
        static constexpr std::uint32_t m_bBombTicking = 0x1170;
        static constexpr std::uint32_t m_nBombSite = 0x1174;
        static constexpr std::uint32_t m_nSourceSoundscapeHash = 0x1178;
        static constexpr std::uint32_t m_entitySpottedState = 0x1180;
        static constexpr std::uint32_t m_flNextGlow = 0x1198;
        static constexpr std::uint32_t m_flNextBeep = 0x119c;
        static constexpr std::uint32_t m_flC4Blow = 0x11a0;
        static constexpr std::uint32_t m_bCannotBeDefused = 0x11a4;
        static constexpr std::uint32_t m_bHasExploded = 0x11a5;
        static constexpr std::uint32_t m_flTimerLength = 0x11a8;
        static constexpr std::uint32_t m_bBeingDefused = 0x11ac;
        static constexpr std::uint32_t m_bTriggerWarning = 0x11b0;
        static constexpr std::uint32_t m_bExplodeWarning = 0x11b4;
        static constexpr std::uint32_t m_bC4Activated = 0x11b8;
        static constexpr std::uint32_t m_bTenSecWarning = 0x11b9;
        static constexpr std::uint32_t m_flDefuseLength = 0x11bc;
        static constexpr std::uint32_t m_flDefuseCountDown = 0x11c0;
        static constexpr std::uint32_t m_bBombDefused = 0x11c4;
        static constexpr std::uint32_t m_hBombDefuser = 0x11c8;
        static constexpr std::uint32_t m_AttributeManager = 0x11d0;
        static constexpr std::uint32_t m_hDefuserMultimeter = 0x16a0;
        static constexpr std::uint32_t m_flNextRadarFlashTime = 0x16a4;
        static constexpr std::uint32_t m_bRadarFlash = 0x16a8;
        static constexpr std::uint32_t m_pBombDefuser = 0x16ac;
        static constexpr std::uint32_t m_fLastDefuseTime = 0x16b0;
        static constexpr std::uint32_t m_pPredictionOwner = 0x16b8;
        static constexpr std::uint32_t m_vecC4ExplodeSpectatePos = 0x16c0;
        static constexpr std::uint32_t m_vecC4ExplodeSpectateAng = 0x16cc;
        static constexpr std::uint32_t m_flC4ExplodeSpectateDuration = 0x16d8;
    };

    class C_Multimeter
    {
    public:
        static constexpr std::uint32_t m_hTargetC4 = 0x1170;
    };

    class C_Item
    {
    public:
        static constexpr std::uint32_t m_pReticleHintTextName = 0x18c0;
    };

    class C_LateUpdatedAnimating
    {
    public:
    };

    class C_CS2HudModelAddon
    {
    public:
    };

    class C_CS2HudModelBase
    {
    public:
    };

    class C_CS2HudModelWeapon
    {
    public:
    };

    class C_CS2HudModelArms
    {
    public:
    };

    class C_HEGrenadeProjectile
    {
    public:
    };

    class C_FlashbangProjectile
    {
    public:
    };

    class C_Chicken
    {
    public:
        static constexpr std::uint32_t m_hHolidayHatAddon = 0x13a8;
        static constexpr std::uint32_t m_jumpedThisFrame = 0x13ac;
        static constexpr std::uint32_t m_leader = 0x13b0;
        static constexpr std::uint32_t m_AttributeManager = 0x13b8;
        static constexpr std::uint32_t m_bAttributesInitialized = 0x1888;
        static constexpr std::uint32_t m_hWaterWakeParticles = 0x188c;
        static constexpr std::uint32_t m_bIsPreviewModel = 0x1890;
    };

    class C_MapPreviewParticleSystem
    {
    public:
    };

    class C_EnvParticleGlow
    {
    public:
        static constexpr std::uint32_t m_flAlphaScale = 0x1438;
        static constexpr std::uint32_t m_flRadiusScale = 0x143c;
        static constexpr std::uint32_t m_flSelfIllumScale = 0x1440;
        static constexpr std::uint32_t m_ColorTint = 0x1444;
        static constexpr std::uint32_t m_hTextureOverride = 0x1448;
    };

    class C_RagdollPropAttached
    {
    public:
        static constexpr std::uint32_t m_boneIndexAttached = 0x11f8;
        static constexpr std::uint32_t m_ragdollAttachedObjectIndex = 0x11fc;
        static constexpr std::uint32_t m_attachmentPointBoneSpace = 0x1200;
        static constexpr std::uint32_t m_attachmentPointRagdollSpace = 0x120c;
        static constexpr std::uint32_t m_vecOffset = 0x1218;
        static constexpr std::uint32_t m_parentTime = 0x1224;
        static constexpr std::uint32_t m_bHasParent = 0x1228;
    };

    class C_BaseCombatCharacter
    {
    public:
        static constexpr std::uint32_t m_hMyWearables = 0x1350;
        static constexpr std::uint32_t m_leftFootAttachment = 0x1368;
        static constexpr std::uint32_t m_rightFootAttachment = 0x1369;
        static constexpr std::uint32_t m_nWaterWakeMode = 0x136c;
        static constexpr std::uint32_t m_flWaterWorldZ = 0x1370;
        static constexpr std::uint32_t m_flWaterNextTraceTime = 0x1374;
    };

    class C_CSWeaponBase
    {
    public:
        static constexpr std::uint32_t m_iWeaponGameplayAnimState = 0x1948;
        static constexpr std::uint32_t m_flWeaponGameplayAnimStateTimestamp = 0x194c;
        static constexpr std::uint32_t m_flInspectCancelCompleteTime = 0x1950;
        static constexpr std::uint32_t m_bInspectPending = 0x1954;
        static constexpr std::uint32_t m_bInspectShouldLoop = 0x1955;
        static constexpr std::uint32_t m_flCrosshairDistance = 0x1980;
        static constexpr std::uint32_t m_iAmmoLastCheck = 0x1984;
        static constexpr std::uint32_t m_nLastEmptySoundCmdNum = 0x1988;
        static constexpr std::uint32_t m_bFireOnEmpty = 0x198c;
        static constexpr std::uint32_t m_OnPlayerPickup = 0x1990;
        static constexpr std::uint32_t m_weaponMode = 0x19a8;
        static constexpr std::uint32_t m_flTurningInaccuracyDelta = 0x19ac;
        static constexpr std::uint32_t m_vecTurningInaccuracyEyeDirLast = 0x19b0;
        static constexpr std::uint32_t m_flTurningInaccuracy = 0x19bc;
        static constexpr std::uint32_t m_fAccuracyPenalty = 0x19c0;
        static constexpr std::uint32_t m_flLastAccuracyUpdateTime = 0x19c4;
        static constexpr std::uint32_t m_fAccuracySmoothedForZoom = 0x19c8;
        static constexpr std::uint32_t m_iRecoilIndex = 0x19cc;
        static constexpr std::uint32_t m_flRecoilIndex = 0x19d0;
        static constexpr std::uint32_t m_bBurstMode = 0x19d4;
        static constexpr std::uint32_t m_flLastBurstModeChangeTime = 0x19d8;
        static constexpr std::uint32_t m_nPostponeFireReadyTicks = 0x19dc;
        static constexpr std::uint32_t m_flPostponeFireReadyFrac = 0x19e0;
        static constexpr std::uint32_t m_bInReload = 0x19e4;
        static constexpr std::uint32_t m_flDroppedAtTime = 0x19e8;
        static constexpr std::uint32_t m_bIsHauledBack = 0x19ec;
        static constexpr std::uint32_t m_bSilencerOn = 0x19ed;
        static constexpr std::uint32_t m_flTimeSilencerSwitchComplete = 0x19f0;
        static constexpr std::uint32_t m_flWeaponActionPlaybackRate = 0x19f4;
        static constexpr std::uint32_t m_iOriginalTeamNumber = 0x19f8;
        static constexpr std::uint32_t m_iMostRecentTeamNumber = 0x19fc;
        static constexpr std::uint32_t m_bDroppedNearBuyZone = 0x1a00;
        static constexpr std::uint32_t m_flNextAttackRenderTimeOffset = 0x1a04;
        static constexpr std::uint32_t m_bClearWeaponIdentifyingUGC = 0x1aa0;
        static constexpr std::uint32_t m_bVisualsDataSet = 0x1aa1;
        static constexpr std::uint32_t m_bUIWeapon = 0x1aa2;
        static constexpr std::uint32_t m_nCustomEconReloadEventId = 0x1aa4;
        static constexpr std::uint32_t m_nextPrevOwnerUseTime = 0x1ab0;
        static constexpr std::uint32_t m_hPrevOwner = 0x1ab4;
        static constexpr std::uint32_t m_nDropTick = 0x1ab8;
        static constexpr std::uint32_t m_bWasActiveWeaponWhenDropped = 0x1abc;
        static constexpr std::uint32_t m_donated = 0x1adc;
        static constexpr std::uint32_t m_fLastShotTime = 0x1ae0;
        static constexpr std::uint32_t m_bWasOwnedByCT = 0x1ae4;
        static constexpr std::uint32_t m_bWasOwnedByTerrorist = 0x1ae5;
        static constexpr std::uint32_t m_flNextClientFireBulletTime = 0x1ae8;
        static constexpr std::uint32_t m_flNextClientFireBulletTime_Repredict = 0x1aec;
        static constexpr std::uint32_t m_IronSightController = 0x1c50;
        static constexpr std::uint32_t m_iIronSightMode = 0x1d00;
        static constexpr std::uint32_t m_flLastLOSTraceFailureTime = 0x1d18;
        static constexpr std::uint32_t m_flWatTickOffset = 0x1d78;
        static constexpr std::uint32_t m_flLastShakeTime = 0x1d8c;
    };

    class C_CSWeaponBaseGun
    {
    public:
        static constexpr std::uint32_t m_zoomLevel = 0x1f40;
        static constexpr std::uint32_t m_iBurstShotsRemaining = 0x1f44;
        static constexpr std::uint32_t m_iSilencerBodygroup = 0x1f48;
        static constexpr std::uint32_t m_silencedModelIndex = 0x1f58;
        static constexpr std::uint32_t m_inPrecache = 0x1f5c;
        static constexpr std::uint32_t m_bNeedsBoltAction = 0x1f5d;
        static constexpr std::uint32_t m_nRevolverCylinderIdx = 0x1f60;
    };

    class C_CSWeaponBaseShotgun
    {
    public:
    };

    class C_C4
    {
    public:
        static constexpr std::uint32_t m_activeLightParticleIndex = 0x1f40;
        static constexpr std::uint32_t m_eActiveLightEffect = 0x1f44;
        static constexpr std::uint32_t m_bStartedArming = 0x1f48;
        static constexpr std::uint32_t m_fArmedTime = 0x1f4c;
        static constexpr std::uint32_t m_bBombPlacedAnimation = 0x1f50;
        static constexpr std::uint32_t m_bIsPlantingViaUse = 0x1f51;
        static constexpr std::uint32_t m_entitySpottedState = 0x1f58;
        static constexpr std::uint32_t m_nSpotRules = 0x1f70;
        static constexpr std::uint32_t m_bPlayedArmingBeeps = 0x1f74;
        static constexpr std::uint32_t m_bBombPlanted = 0x1f7b;
    };

    class C_WeaponCZ75a
    {
    public:
        static constexpr std::uint32_t m_bMagazineRemoved = 0x1f70;
    };

    class C_DEagle
    {
    public:
    };

    class C_WeaponElite
    {
    public:
    };

    class C_WeaponTaser
    {
    public:
        static constexpr std::uint32_t m_fFireTime = 0x1f70;
        static constexpr std::uint32_t m_nLastAttackTick = 0x1f74;
    };

    class C_Knife
    {
    public:
        static constexpr std::uint32_t m_bFirstAttack = 0x1f40;
    };

    class C_MolotovProjectile
    {
    public:
        static constexpr std::uint32_t m_bIsIncGrenade = 0x1438;
    };

    class C_DecoyProjectile
    {
    public:
        static constexpr std::uint32_t m_nDecoyShotTick = 0x1438;
        static constexpr std::uint32_t m_nClientLastKnownDecoyShotTick = 0x143c;
        static constexpr std::uint32_t m_flTimeParticleEffectSpawn = 0x1460;
    };

    class C_SmokeGrenadeProjectile
    {
    public:
        static constexpr std::uint32_t m_nSmokeEffectTickBegin = 0x1450;
        static constexpr std::uint32_t m_bDidSmokeEffect = 0x1454;
        static constexpr std::uint32_t m_nRandomSeed = 0x1458;
        static constexpr std::uint32_t m_vSmokeColor = 0x145c;
        static constexpr std::uint32_t m_vSmokeDetonationPos = 0x1468;
        static constexpr std::uint32_t m_VoxelFrameData = 0x1478;
        static constexpr std::uint32_t m_nVoxelFrameDataSize = 0x1490;
        static constexpr std::uint32_t m_nVoxelUpdate = 0x1494;
        static constexpr std::uint32_t m_bSmokeVolumeDataReceived = 0x1498;
        static constexpr std::uint32_t m_bSmokeEffectSpawned = 0x1499;
    };

    class C_BaseCSGrenade
    {
    public:
        static constexpr std::uint32_t m_bClientPredictDelete = 0x1f40;
        static constexpr std::uint32_t m_bRedraw = 0x1f41;
        static constexpr std::uint32_t m_bIsHeldByPlayer = 0x1f42;
        static constexpr std::uint32_t m_bPinPulled = 0x1f43;
        static constexpr std::uint32_t m_bJumpThrow = 0x1f44;
        static constexpr std::uint32_t m_bThrowAnimating = 0x1f45;
        static constexpr std::uint32_t m_fThrowTime = 0x1f48;
        static constexpr std::uint32_t m_flThrowStrength = 0x1f50;
        static constexpr std::uint32_t m_fDropTime = 0x1fc8;
        static constexpr std::uint32_t m_fPinPullTime = 0x1fcc;
        static constexpr std::uint32_t m_bJustPulledPin = 0x1fd0;
        static constexpr std::uint32_t m_nNextHoldTick = 0x1fd4;
        static constexpr std::uint32_t m_flNextHoldFrac = 0x1fd8;
        static constexpr std::uint32_t m_hSwitchToWeaponAfterThrow = 0x1fdc;
    };

    class C_WeaponBaseItem
    {
    public:
        static constexpr std::uint32_t m_bSequenceInProgress = 0x1f40;
        static constexpr std::uint32_t m_bRedraw = 0x1f41;
    };

    class C_ItemDogtags
    {
    public:
        static constexpr std::uint32_t m_OwningPlayer = 0x19c0;
        static constexpr std::uint32_t m_KillingPlayer = 0x19c4;
    };

    class C_Item_Healthshot
    {
    public:
    };

    class C_CSPlayerPawnBase
    {
    public:
        static constexpr std::uint32_t m_pPingServices = 0x15c0;
        static constexpr std::uint32_t m_previousPlayerState = 0x15c8;
        static constexpr std::uint32_t m_iPlayerState = 0x15cc;
        static constexpr std::uint32_t m_bHasMovedSinceSpawn = 0x15d0;
        static constexpr std::uint32_t m_flLastSpawnTimeIndex = 0x15d4;
        static constexpr std::uint32_t m_iProgressBarDuration = 0x15d8;
        static constexpr std::uint32_t m_flProgressBarStartTime = 0x15dc;
        static constexpr std::uint32_t m_flClientDeathTime = 0x15e0;
        static constexpr std::uint32_t m_flFlashBangTime = 0x15e4;
        static constexpr std::uint32_t m_flFlashScreenshotAlpha = 0x15e8;
        static constexpr std::uint32_t m_flFlashOverlayAlpha = 0x15ec;
        static constexpr std::uint32_t m_bFlashBuildUp = 0x15f0;
        static constexpr std::uint32_t m_bFlashDspHasBeenCleared = 0x15f1;
        static constexpr std::uint32_t m_bFlashScreenshotHasBeenGrabbed = 0x15f2;
        static constexpr std::uint32_t m_flFlashMaxAlpha = 0x15f4;
        static constexpr std::uint32_t m_flFlashDuration = 0x15f8;
        static constexpr std::uint32_t m_flClientHealthFadeChangeTimestamp = 0x15fc;
        static constexpr std::uint32_t m_nClientHealthFadeParityValue = 0x1600;
        static constexpr std::uint32_t m_fNextThinkPushAway = 0x1604;
        static constexpr std::uint32_t m_flCurrentMusicStartTime = 0x160c;
        static constexpr std::uint32_t m_flMusicRoundStartTime = 0x1610;
        static constexpr std::uint32_t m_bDeferStartMusicOnWarmup = 0x1614;
        static constexpr std::uint32_t m_flLastSmokeOverlayAlpha = 0x1618;
        static constexpr std::uint32_t m_flLastSmokeAge = 0x161c;
        static constexpr std::uint32_t m_vLastSmokeOverlayColor = 0x1620;
        static constexpr std::uint32_t m_hOriginalController = 0x1648;
    };

    class C_Hostage
    {
    public:
        static constexpr std::uint32_t m_entitySpottedState = 0x13d8;
        static constexpr std::uint32_t m_leader = 0x13f0;
        static constexpr std::uint32_t m_reuseTimer = 0x13f8;
        static constexpr std::uint32_t m_vel = 0x1410;
        static constexpr std::uint32_t m_isRescued = 0x141c;
        static constexpr std::uint32_t m_jumpedThisFrame = 0x141d;
        static constexpr std::uint32_t m_nHostageState = 0x1420;
        static constexpr std::uint32_t m_bHandsHaveBeenCut = 0x1424;
        static constexpr std::uint32_t m_hHostageGrabber = 0x1428;
        static constexpr std::uint32_t m_fLastGrabTime = 0x142c;
        static constexpr std::uint32_t m_vecGrabbedPos = 0x1430;
        static constexpr std::uint32_t m_flRescueStartTime = 0x143c;
        static constexpr std::uint32_t m_flGrabSuccessTime = 0x1440;
        static constexpr std::uint32_t m_flDropStartTime = 0x1444;
        static constexpr std::uint32_t m_flDeadOrRescuedTime = 0x1448;
        static constexpr std::uint32_t m_blinkTimer = 0x1450;
        static constexpr std::uint32_t m_lookAt = 0x1468;
        static constexpr std::uint32_t m_lookAroundTimer = 0x1478;
        static constexpr std::uint32_t m_isInit = 0x1490;
        static constexpr std::uint32_t m_eyeAttachment = 0x1491;
        static constexpr std::uint32_t m_chestAttachment = 0x1492;
        static constexpr std::uint32_t m_pPredictionOwner = 0x1498;
        static constexpr std::uint32_t m_fNewestAlphaThinkTime = 0x14a0;
    };

    class C_NetTestBaseCombatCharacter
    {
    public:
    };

    class C_AK47
    {
    public:
    };

    class C_WeaponAug
    {
    public:
    };

    class C_WeaponAWP
    {
    public:
    };

    class C_WeaponBizon
    {
    public:
    };

    class C_WeaponFamas
    {
    public:
    };

    class C_WeaponFiveSeven
    {
    public:
    };

    class C_WeaponG3SG1
    {
    public:
    };

    class C_WeaponGalilAR
    {
    public:
    };

    class C_WeaponGlock
    {
    public:
    };

    class C_WeaponHKP2000
    {
    public:
    };

    class C_WeaponUSPSilencer
    {
    public:
    };

    class C_WeaponM4A1
    {
    public:
    };

    class C_WeaponM4A1Silencer
    {
    public:
    };

    class C_WeaponMAC10
    {
    public:
    };

    class C_WeaponMag7
    {
    public:
    };

    class C_WeaponMP5SD
    {
    public:
    };

    class C_WeaponMP7
    {
    public:
    };

    class C_WeaponMP9
    {
    public:
    };

    class C_WeaponNegev
    {
    public:
    };

    class C_WeaponP250
    {
    public:
    };

    class C_WeaponP90
    {
    public:
    };

    class C_WeaponSCAR20
    {
    public:
    };

    class C_WeaponSG556
    {
    public:
    };

    class C_WeaponSSG08
    {
    public:
    };

    class C_WeaponTec9
    {
    public:
    };

    class C_WeaponUMP45
    {
    public:
    };

    class C_WeaponM249
    {
    public:
    };

    class C_WeaponRevolver
    {
    public:
    };

    class C_WeaponNOVA
    {
    public:
    };

    class C_WeaponSawedoff
    {
    public:
    };

    class C_WeaponXM1014
    {
    public:
    };

    class C_MolotovGrenade
    {
    public:
    };

    class C_IncendiaryGrenade
    {
    public:
    };

    class C_DecoyGrenade
    {
    public:
    };

    class C_Flashbang
    {
    public:
    };

    class C_HEGrenade
    {
    public:
    };

    class C_SmokeGrenade
    {
    public:
    };

    class C_CSGO_PreviewPlayer
    {
    public:
        static constexpr std::uint32_t m_animgraphCharacterModeString = 0x3ef0;
        static constexpr std::uint32_t m_flInitialModelScale = 0x3ef8;
    };

    class C_CSGO_PreviewPlayerAlias_csgo_player_previewmodel
    {
    public:
    };

    class C_CSGO_TeamPreviewModel
    {
    public:
    };
}
