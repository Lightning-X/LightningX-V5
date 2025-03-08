#include "Hooking.hpp"
#include "Notify.hpp"
#include "Translations.hpp"
#include "GtaUtill.hpp"
#include "GTAV-Classes-master/rage/joaat.hpp"
#include "joaat2.hpp"
#include "RobinHood.hpp"
#include "GTAV-Classes-master/netsync/nodes/vehicle/CVehicleCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/door/CDoorCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/pickup/CPickupCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/physical/CPhysicalAttachDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/ped/CPedCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/ped/CPedAttachDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/object/CObjectCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerAppearanceDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerCreationDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/proximity_migrateable/CSectorDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/vehicle/CVehicleProximityMigrationDataNode.hpp"
#include "GTAV-Classes-master/vehicle/CVehicleModelInfo.hpp"
#include "GTAV-Classes-master/base/CObject.hpp"
#include "GTAV-Classes-master/netsync/nodes/train/CTrainGameStateDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/player/CPlayerGamerDataNode.hpp"
#include "GTAV-Classes-master/netsync/nodes/ped/CPedGameStateDataNode.hpp"
namespace base
{
	// iterate all models
	/*
	for (int i = 0; i < m_model_table->m_size; ++i)
	{
		for (auto node = m_model_table->m_lookup_table[i]; node; node = node->m_next)
		{
			if (const auto table_idx = node->m_idx; table_idx < m_model_table->m_size)
			{
				if (const auto model = m_model_table->m_data[table_idx]; model && model->m_model_type == eModelType::Vehicle)
				{
				}
			}
		}
	}*/

	class model_info
	{
	public:
		static bool does_model_exist(const rage::joaat_t hash)
		{
			if (const auto model = model_info::get_model(hash); model)
				return true;
			return false;
		}

		template<typename T = CBaseModelInfo*>
		static T get_model(const rage::joaat_t hash)
		{
			const auto model_table = g_pointers->m_modelTable;
			for (auto i = model_table->m_lookup_table[hash % model_table->m_lookup_key]; i; i = i->m_next)
			{
				if (i->m_hash == hash)
				{
					if (const auto model = model_table->m_data[i->m_idx]; model)
					{
						return reinterpret_cast<T>(model);
					}
				}
			}
			return nullptr;
		}

		static CVehicleModelInfo* get_vehicle_model(const rage::joaat_t hash)
		{
			if (const auto model = model_info::get_model<CVehicleModelInfo*>(hash); model && model->is_type(eModelType::Vehicle))
				return model;
			return nullptr;
		}

		template<typename T, typename... Args>
		static bool is_model_of_type(const rage::joaat_t hash, const T arg, const Args... args)
		{
			bool of_type = false;
			if (const auto model = model_info::get_model(hash))
			{
				of_type = model->is_type(arg);
				(
					[&of_type, &model](eModelType type) {
						of_type |= model->is_type(type);
					}(args),
						...);
			}
			return of_type;
		}
	};

	struct sync_node_id
	{
		Hash id;

		constexpr sync_node_id()
		{
			id = 0;
		}

		template<size_t N>
		constexpr sync_node_id(char const (&pp)[N])
		{
			id = rage::consteval_joaat(pp);
		}

		// implicit conversion
		constexpr operator Hash() const
		{
			return id;
		}
	};

	using sync_node_vft = uintptr_t;

	// Sync Node Virtual Function Table address to all sync node identifiers that also have that vft address.
	using sync_node_vft_to_ids = ankerl::unordered_dense::map<uint64_t, sync_node_id>;

	// Sync Tree node array index to node identifier.
	using sync_tree_node_array_index_to_node_id_t = std::vector<sync_node_id>;

	struct sync_node_finder_t
	{
		static constexpr size_t sync_tree_count = size_t(eNetObjType::NET_OBJ_TYPE_TRAIN) + 1;

		std::array<sync_node_vft_to_ids, sync_tree_count> sync_trees_sync_node_addr_to_ids;

		std::array<sync_tree_node_array_index_to_node_id_t, sync_tree_count> sync_trees_node_array_index_to_node_id =
		{
			{
				// AUTOMOBILE
				{
					{"CVehicleCreationDataNode"},
					{"CAutomobileCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// BIKE
				{
					{"CVehicleCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},
					{"CBikeGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// BOAT
				{
					{"CVehicleCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},
					{"CBoatGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// DOOR
				{
					{"CDoorCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDoorScriptInfoDataNode"},
					{"CDoorScriptGameStateDataNode"},

					{"CDoorMovementDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
				},
				// HELI
				{
					{"CVehicleCreationDataNode"},
					{"CAutomobileCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},
					{"CHeliHealthDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},
					{"CHeliControlDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// OBJECT
				{
					{"CObjectCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CObjectGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CObjectScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CPhysicalHealthDataNode"},

					{"CSectorDataNode"},
					{"CObjectSectorPosNode"},
					{"CObjectOrientationNode"},
					{"CPhysicalVelocityDataNode"},
					{"CPhysicalAngVelocityDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
				},
				// PED
				{
					{"CPedCreationDataNode"},
					{"CPedScriptCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CPedGameStateDataNode"},
					{"CPedComponentReservationDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CPedScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPedAttachDataNode"},
					{"CPedHealthDataNode"},
					{"CPedMovementGroupDataNode"},
					{"CPedAIDataNode"},
					{"CPedAppearanceDataNode"},

					{"CPedOrientationDataNode"},
					{"CPedMovementDataNode"},

					{"CPedTaskTreeDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},

					{"CSectorDataNode"},
					{"CPedSectorPosMapNode"},
					{"CPedSectorPosNavMeshNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CPedInventoryDataNode"},
					{"CPedTaskSequenceDataNode"},
				},
				// PICKUP
				{
					{"CPickupCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},

					{"CPickupScriptGameStateNode"},
					{"CPhysicalGameStateDataNode"},
					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},
					{"CPhysicalHealthDataNode"},

					{"CPhysicalAttachDataNode"},

					{"CSectorDataNode"},
					{"CPickupSectorPosNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CPhysicalAngVelocityDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
				},
				// PICKUP_PLACEMENT
				{
					{"CPickupPlacementCreationDataNode"},
					{"CMigrationDataNode"},
					{"CGlobalFlagsDataNode"},
					{"CPickupPlacementStateDataNode"},
				},
				// PLANE
				{
					{"CVehicleCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},
					{"CPlaneGameStateDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},
					{"CPlaneControlDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// SUBMARINE
				{
					{"CVehicleCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},
					{"CSubmarineGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},
					{"CSubmarineControlDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
				// PLAYER
				{
					{"CPlayerCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CPedGameStateDataNode"},
					{"CPedComponentReservationDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPlayerGameStateDataNode"},

					{"CPedAttachDataNode"},
					{"CPedHealthDataNode"},
					{"CPedMovementGroupDataNode"},
					{"CPedAIDataNode"},
					{"CPlayerAppearanceDataNode"},
					{"CPlayerPedGroupDataNode"},
					{"CPlayerAmbientModelStreamingNode"},
					{"CPlayerGamerDataNode"},
					{"CPlayerExtendedGameStateNode"},

					{"CPedOrientationDataNode"},
					{"CPedMovementDataNode"},

					{"CPedTaskTreeDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},
					{"CPedTaskSpecificDataNode"},

					{"CSectorDataNode"},
					{"CPlayerSectorPosNode"},
					{"CPlayerCameraDataNode"},
					{"CPlayerWantedAndLOSDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
				},
				// TRAILER
				{
				},
				// TRAIN
				{
					{"CVehicleCreationDataNode"},

					{"CGlobalFlagsDataNode"},
					{"CDynamicEntityGameStateDataNode"},
					{"CPhysicalGameStateDataNode"},
					{"CVehicleGameStateDataNode"},
					{"CTrainGameStateDataNode"},

					{"CEntityScriptGameStateDataNode"},
					{"CPhysicalScriptGameStateDataNode"},
					{"CVehicleScriptGameStateDataNode"},
					{"CEntityScriptInfoDataNode"},

					{"CPhysicalAttachDataNode"},
					{"CVehicleAppearanceDataNode"},
					{"CVehicleDamageStatusDataNode"},
					{"CVehicleComponentReservationDataNode"},
					{"CVehicleHealthDataNode"},
					{"CVehicleTaskDataNode"},

					{"CSectorDataNode"},
					{"CSectorPositionDataNode"},
					{"CEntityOrientationDataNode"},
					{"CPhysicalVelocityDataNode"},
					{"CVehicleAngVelocityDataNode"},

					{"CVehicleSteeringDataNode"},
					{"CVehicleControlDataNode"},
					{"CVehicleGadgetDataNode"},

					{"CMigrationDataNode"},
					{"CPhysicalMigrationDataNode"},
					{"CPhysicalScriptMigrationDataNode"},
					{"CVehicleProximityMigrationDataNode"},
				},
			},
		};
	};

	class sync_node_finder
	{
	private:

		static inline sync_node_finder_t finder;

	public:
		static const sync_node_id& find(eNetObjType obj_type, uintptr_t addr)
		{
			return finder.sync_trees_sync_node_addr_to_ids[(int)obj_type][addr];
		}

		static void init()
		{
			for (int i = (int)eNetObjType::NET_OBJ_TYPE_AUTOMOBILE; i <= (int)eNetObjType::NET_OBJ_TYPE_TRAIN; i++)
			{
				if (i == (int)eNetObjType::NET_OBJ_TYPE_TRAILER)
					continue;

				rage::netSyncTree* tree = g_pointers->m_get_sync_tree_for_type(*g_pointers->m_network_object_mgr, i);

				if (tree->m_child_node_count != finder.sync_trees_node_array_index_to_node_id[i].size())
				{
					g_logger->send(red, "CAD", "Cache nodes failed: {} | {}", i, tree->m_child_node_count, finder.sync_trees_node_array_index_to_node_id[i].size());
					throw std::runtime_error("Failed to cache nodes");
				}

				for (int j = 0; j < tree->m_child_node_count; j++)
				{
					const uintptr_t addr = (uintptr_t)tree->m_child_nodes[j];

					const sync_node_id node_id = finder.sync_trees_node_array_index_to_node_id[i][j];

					finder.sync_trees_sync_node_addr_to_ids[i][addr] = node_id;
				}
			}
		}
	};

	const inline static std::vector<std::pair<int, const char*>> task_type_names = {
	{0, "CTaskHandsUp"},
	{1, "CTaskClimbLadder"},
	{2, "CTaskExitVehicle"},
	{3, "CTaskCombatRoll"},
	{4, "CTaskAimGunOnFoot"},
	{5, "CTaskMovePlayer"},
	{6, "CTaskPlayerOnFoot"},
	{8, "CTaskWeapon"},
	{9, "CTaskPlayerWeapon"},
	{10, "CTaskPlayerIdles"},
	{12, "CTaskAimGun"},
	{12, "CTaskComplex"},
	{12, "CTaskFSMClone"},
	{12, "CTaskMotionBase"},
	{12, "CTaskMove"},
	{12, "CTaskMoveBase"},
	{12, "CTaskNMBehaviour"},
	{12, "CTaskNavBase"},
	{12, "CTaskScenario"},
	{12, "CTaskSearchBase"},
	{12, "CTaskSearchInVehicleBase"},
	{12, "CTaskShockingEvent"},
	{12, "CTaskTrainBase"},
	{12, "CTaskVehicleFSM"},
	{12, "CTaskVehicleGoTo"},
	{12, "CTaskVehicleMissionBase"},
	{12, "CTaskVehicleTempAction"},
	{14, "CTaskPause"},
	{15, "CTaskDoNothing"},
	{16, "CTaskGetUp"},
	{17, "CTaskGetUpAndStandStill"},
	{18, "CTaskFallOver"},
	{19, "CTaskFallAndGetUp"},
	{20, "CTaskCrawl"},
	{25, "CTaskComplexOnFire"},
	{26, "CTaskDamageElectric"},
	{28, "CTaskTriggerLookAt"},
	{29, "CTaskClearLookAt"},
	{30, "CTaskSetCharDecisionMaker"},
	{31, "CTaskSetPedDefensiveArea"},
	{32, "CTaskUseSequence"},
	{34, "CTaskMoveStandStill"},
	{35, "CTaskComplexControlMovement"},
	{36, "CTaskMoveSequence"},
	{38, "CTaskAmbientClips"},
	{39, "CTaskMoveInAir"},
	{40, "CTaskNetworkClone"},
	{41, "CTaskUseClimbOnRoute"},
	{42, "CTaskUseDropDownOnRoute"},
	{43, "CTaskUseLadderOnRoute"},
	{44, "CTaskSetBlockingOfNonTemporaryEvents"},
	{45, "CTaskForceMotionState"},
	{46, "CTaskSlopeScramble"},
	{47, "CTaskGoToAndClimbLadder"},
	{48, "CTaskClimbLadderFully"},
	{49, "CTaskRappel"},
	{50, "CTaskVault"},
	{51, "CTaskDropDown"},
	{52, "CTaskAffectSecondaryBehaviour"},
	{53, "CTaskAmbientLookAtEvent"},
	{54, "CTaskOpenDoor"},
	{55, "CTaskShovePed"},
	{56, "CTaskSwapWeapon"},
	{57, "CTaskGeneralSweep"},
	{58, "CTaskPolice"},
	{59, "CTaskPoliceOrderResponse"},
	{60, "CTaskPursueCriminal"},
	{62, "CTaskArrestPed"},
	{63, "CTaskArrestPed2"},
	{64, "CTaskBusted"},
	{65, "CTaskFirePatrol"},
	{66, "CTaskHeliOrderResponse"},
	{67, "CTaskHeliPassengerRappel"},
	{68, "CTaskAmbulancePatrol"},
	{69, "CTaskPoliceWantedResponse"},
	{70, "CTaskSwat"},
	{72, "CTaskSwatWantedResponse"},
	{73, "CTaskSwatOrderResponse"},
	{74, "CTaskSwatGoToStagingArea"},
	{75, "CTaskSwatFollowInLine"},
	{76, "CTaskWitness"},
	{77, "CTaskGangPatrol"},
	{78, "CTaskArmy"},
	{80, "CTaskShockingEventWatch"},
	{82, "CTaskShockingEventGoto"},
	{83, "CTaskShockingEventHurryAway"},
	{84, "CTaskShockingEventReactToAircraft"},
	{85, "CTaskShockingEventReact"},
	{86, "CTaskShockingEventBackAway"},
	{87, "CTaskShockingPoliceInvestigate"},
	{88, "CTaskShockingEventStopAndStare"},
	{89, "CTaskShockingNiceCarPicture"},
	{90, "CTaskShockingEventThreatResponse"},
	{92, "CTaskTakeOffHelmet"},
	{93, "CTaskCarReactToVehicleCollision"},
	{95, "CTaskCarReactToVehicleCollisionGetOut"},
	{97, "CTaskDyingDead"},
	{100, "CTaskWanderingScenario"},
	{101, "CTaskWanderingInRadiusScenario"},
	{103, "CTaskMoveBetweenPointsScenario"},
	{104, "CTaskChatScenario"},
	{106, "CTaskCowerScenario"},
	{107, "CTaskDeadBodyScenario"},
	{114, "CTaskSayAudio"},
	{116, "CTaskWaitForSteppingOut"},
	{117, "CTaskCoupleScenario"},
	{118, "CTaskUseScenario"},
	{119, "CTaskUseVehicleScenario"},
	{120, "CTaskUnalerted"},
	{121, "CTaskStealVehicle"},
	{122, "CTaskReactToPursuit"},
	{125, "CTaskHitWall"},
	{126, "CTaskCower"},
	{127, "CTaskCrouch"},
	{128, "CTaskMelee"},
	{129, "CTaskMoveMeleeMovement"},
	{130, "CTaskMeleeActionResult"},
	{131, "CTaskMeleeUpperbodyAnims"},
	{133, "CTaskMoVEScripted"},
	{134, "CTaskScriptedAnimation"},
	{135, "CTaskSynchronizedScene"},
	{137, "CTaskComplexEvasiveStep"},
	{138, "CTaskWalkRoundCarWhileWandering"},
	{140, "CTaskComplexStuckInAir"},
	{141, "CTaskWalkRoundEntity"},
	{142, "CTaskMoveWalkRoundVehicle"},
	{144, "CTaskReactToGunAimedAt"},
	{146, "CTaskDuckAndCover"},
	{147, "CTaskAggressiveRubberneck"},
	{150, "CTaskInVehicleBasic"},
	{151, "CTaskCarDriveWander"},
	{152, "CTaskLeaveAnyCar"},
	{153, "CTaskComplexGetOffBoat"},
	{155, "CTaskCarSetTempAction"},
	{156, "CTaskBringVehicleToHalt"},
	{157, "CTaskCarDrive"},
	{159, "CTaskPlayerDrive"},
	{160, "CTaskEnterVehicle"},
	{161, "CTaskEnterVehicleAlign"},
	{162, "CTaskOpenVehicleDoorFromOutside"},
	{163, "CTaskEnterVehicleSeat"},
	{164, "CTaskCloseVehicleDoorFromInside"},
	{165, "CTaskInVehicleSeatShuffle"},
	{167, "CTaskExitVehicleSeat"},
	{168, "CTaskCloseVehicleDoorFromOutside"},
	{169, "CTaskControlVehicle"},
	{170, "CTaskMotionInAutomobile"},
	{171, "CTaskMotionOnBicycle"},
	{172, "CTaskMotionOnBicycleController"},
	{173, "CTaskMotionInVehicle"},
	{174, "CTaskMotionInTurret"},
	{175, "CTaskReactToBeingJacked"},
	{176, "CTaskReactToBeingAskedToLeaveVehicle"},
	{177, "CTaskTryToGrabVehicleDoor"},
	{178, "CTaskGetOnTrain"},
	{179, "CTaskGetOffTrain"},
	{180, "CTaskRideTrain"},
	{190, "CTaskMountThrowProjectile"},
	{195, "CTaskGoToCarDoorAndStandStill"},
	{196, "CTaskMoveGoToVehicleDoor"},
	{197, "CTaskSetPedInVehicle"},
	{198, "CTaskSetPedOutOfVehicle"},
	{199, "CTaskVehicleMountedWeapon"},
	{200, "CTaskVehicleGun"},
	{201, "CTaskVehicleProjectile"},
	{204, "CTaskSmashCarWindow"},
	{205, "CTaskMoveGoToPoint"},
	{206, "CTaskMoveAchieveHeading"},
	{207, "CTaskMoveFaceTarget"},
	{208, "CTaskComplexGoToPointAndStandStillTimed"},
	{208, "CTaskMoveGoToPointAndStandStill"},
	{209, "CTaskMoveFollowPointRoute"},
	{210, "CTaskMoveSeekEntity_CEntitySeekPosCalculatorStandard"},
	{211, "CTaskMoveSeekEntity_CEntitySeekPosCalculatorLastNavMeshIntersection"},
	{212, "CTaskMoveSeekEntity_CEntitySeekPosCalculatorLastNavMeshIntersection2"},
	{213, "CTaskMoveSeekEntity_CEntitySeekPosCalculatorXYOffsetFixed"},
	{214, "CTaskMoveSeekEntity_CEntitySeekPosCalculatorXYOffsetFixed2"},
	{215, "CTaskExhaustedFlee"},
	{216, "CTaskGrowlAndFlee"},
	{217, "CTaskScenarioFlee"},
	{218, "CTaskSmartFlee"},
	{219, "CTaskFlyAway"},
	{220, "CTaskWalkAway"},
	{221, "CTaskWander"},
	{222, "CTaskWanderInArea"},
	{223, "CTaskFollowLeaderInFormation"},
	{224, "CTaskGoToPointAnyMeans"},
	{225, "CTaskTurnToFaceEntityOrCoord"},
	{226, "CTaskFollowLeaderAnyMeans"},
	{228, "CTaskFlyToPoint"},
	{229, "CTaskFlyingWander"},
	{230, "CTaskGoToPointAiming"},
	{231, "CTaskGoToScenario"},
	{233, "CTaskSeekEntityAiming"},
	{234, "CTaskSlideToCoord"},
	{235, "CTaskSwimmingWander"},
	{237, "CTaskMoveTrackingEntity"},
	{238, "CTaskMoveFollowNavMesh"},
	{239, "CTaskMoveGoToPointOnRoute"},
	{240, "CTaskEscapeBlast"},
	{241, "CTaskMoveWander"},
	{242, "CTaskMoveBeInFormation"},
	{243, "CTaskMoveCrowdAroundLocation"},
	{244, "CTaskMoveCrossRoadAtTrafficLights"},
	{245, "CTaskMoveWaitForTraffic"},
	{246, "CTaskMoveGoToPointStandStillAchieveHeading"},
	{251, "CTaskMoveGetOntoMainNavMesh"},
	{252, "CTaskMoveSlideToCoord"},
	{253, "CTaskMoveGoToPointRelativeToEntityAndStandStill"},
	{254, "CTaskHelicopterStrafe"},
	{256, "CTaskGetOutOfWater"},
	{259, "CTaskMoveFollowEntityOffset"},
	{261, "CTaskFollowWaypointRecording"},
	{264, "CTaskMotionPed"},
	{265, "CTaskMotionPedLowLod"},
	{268, "CTaskHumanLocomotion"},
	{269, "CTaskMotionBasicLocomotionLowLod"},
	{270, "CTaskMotionStrafing"},
	{271, "CTaskMotionTennis"},
	{272, "CTaskMotionAiming"},
	{273, "CTaskBirdLocomotion"},
	{274, "CTaskFlightlessBirdLocomotion"},
	{278, "CTaskFishLocomotion"},
	{279, "CTaskQuadLocomotion"},
	{280, "CTaskMotionDiving"},
	{281, "CTaskMotionSwimming"},
	{282, "CTaskMotionParachuting"},
	{283, "CTaskMotionDrunk"},
	{284, "CTaskRepositionMove"},
	{285, "CTaskMotionAimingTransition"},
	{286, "CTaskThrowProjectile"},
	{287, "CTaskCover"},
	{288, "CTaskMotionInCover"},
	{289, "CTaskAimAndThrowProjectile"},
	{290, "CTaskGun"},
	{291, "CTaskAimFromGround"},
	{295, "CTaskAimGunVehicleDriveBy"},
	{296, "CTaskAimGunScripted"},
	{298, "CTaskReloadGun"},
	{299, "CTaskWeaponBlocked"},
	{300, "CTaskEnterCover"},
	{301, "CTaskExitCover"},
	{302, "CTaskAimGunFromCoverIntro"},
	{303, "CTaskAimGunFromCoverOutro"},
	{304, "CTaskAimGunBlindFire"},
	{307, "CTaskCombatClosestTargetInArea"},
	{308, "CTaskCombatAdditionalTask"},
	{309, "CTaskInCover"},
	{313, "CTaskAimSweep"},
	{319, "CTaskSharkCircle"},
	{320, "CTaskSharkAttack"},
	{321, "CTaskAgitated"},
	{322, "CTaskAgitatedAction"},
	{323, "CTaskConfront"},
	{324, "CTaskIntimidate"},
	{325, "CTaskShove"},
	{326, "CTaskShoved"},
	{328, "CTaskCrouchToggle"},
	{329, "CTaskRevive"},
	{335, "CTaskParachute"},
	{336, "CTaskParachuteObject"},
	{337, "CTaskTakeOffPedVariation"},
	{340, "CTaskCombatSeekCover"},
	{342, "CTaskCombatFlank"},
	{343, "CTaskCombat"},
	{344, "CTaskCombatMounted"},
	{345, "CTaskMoveCircle"},
	{346, "CTaskMoveCombatMounted"},
	{347, "CTaskSearch"},
	{348, "CTaskSearchOnFoot"},
	{349, "CTaskSearchInAutomobile"},
	{350, "CTaskSearchInBoat"},
	{351, "CTaskSearchInHeli"},
	{352, "CTaskThreatResponse"},
	{353, "CTaskInvestigate"},
	{354, "CTaskStandGuardFSM"},
	{355, "CTaskPatrol"},
	{356, "CTaskShootAtTarget"},
	{357, "CTaskSetAndGuardArea"},
	{358, "CTaskStandGuard"},
	{359, "CTaskSeparate"},
	{360, "CTaskStayInCover"},
	{361, "CTaskVehicleCombat"},
	{362, "CTaskVehiclePersuit"},
	{363, "CTaskVehicleChase"},
	{364, "CTaskDraggingToSafety"},
	{365, "CTaskDraggedToSafety"},
	{366, "CTaskVariedAimPose"},
	{367, "CTaskMoveWithinAttackWindow"},
	{368, "CTaskMoveWithinDefensiveArea"},
	{369, "CTaskShootOutTire"},
	{370, "CTaskShellShocked"},
	{371, "CTaskBoatChase"},
	{372, "CTaskBoatCombat"},
	{373, "CTaskBoatStrafe"},
	{374, "CTaskHeliChase"},
	{375, "CTaskHeliCombat"},
	{376, "CTaskSubmarineCombat"},
	{377, "CTaskSubmarineChase"},
	{378, "CTaskPlaneChase"},
	{379, "CTaskTargetUnreachable"},
	{380, "CTaskTargetUnreachableInInterior"},
	{381, "CTaskTargetUnreachableInExterior"},
	{382, "CTaskStealthKill"},
	{383, "CTaskWrithe"},
	{384, "CTaskAdvance"},
	{385, "CTaskCharge"},
	{386, "CTaskMoveToTacticalPoint"},
	{387, "CTaskToHurtTransit"},
	{388, "CTaskAnimatedHitByExplosion"},
	{389, "CTaskNMRelax"},
	{391, "CTaskNMPose"},
	{392, "CTaskNMBrace"},
	{393, "CTaskNMBuoyancy"},
	{394, "CTaskNMInjuredOnGround"},
	{395, "CTaskNMShot"},
	{396, "CTaskNMHighFall"},
	{397, "CTaskNMBalance"},
	{398, "CTaskNMElectrocute"},
	{399, "CTaskNMPrototype"},
	{400, "CTaskNMExplosion"},
	{401, "CTaskNMOnFire"},
	{402, "CTaskNMScriptControl"},
	{403, "CTaskNMJumpRollFromRoadVehicle"},
	{404, "CTaskNMFlinch"},
	{405, "CTaskNMSit"},
	{406, "CTaskNMFallDown"},
	{407, "CTaskBlendFromNM"},
	{408, "CTaskNMControl"},
	{409, "CTaskNMDangle"},
	{412, "CTaskNMGenericAttach"},
	{414, "CTaskNMDraggingToSafety"},
	{415, "CTaskNMThroughWindscreen"},
	{416, "CTaskNMRiverRapids"},
	{417, "CTaskNMSimple"},
	{418, "CTaskRageRagdoll"},
	{421, "CTaskJumpVault"},
	{422, "CTaskJump"},
	{423, "CTaskFall"},
	{425, "CTaskReactAimWeapon"},
	{426, "CTaskChat"},
	{427, "CTaskMobilePhone"},
	{428, "CTaskReactToDeadPed"},
	{430, "CTaskSearchForUnknownThreat"},
	{432, "CTaskBomb"},
	{433, "CTaskDetonator"},
	{435, "CTaskAnimatedAttach"},
	{441, "CTaskCutScene"},
	{442, "CTaskReactToExplosion"},
	{443, "CTaskReactToImminentExplosion"},
	{444, "CTaskDiveToGround"},
	{445, "CTaskReactAndFlee"},
	{446, "CTaskSidestep"},
	{447, "CTaskCallPolice"},
	{448, "CTaskReactInDirection"},
	{449, "CTaskReactToBuddyShot"},
	{454, "CTaskVehicleGoToAutomobileNew"},
	{455, "CTaskVehicleGoToPlane"},
	{456, "CTaskVehicleGoToHelicopter"},
	{457, "CTaskVehicleGoToSubmarine"},
	{458, "CTaskVehicleGoToBoat"},
	{459, "CTaskVehicleGoToPointAutomobile"},
	{460, "CTaskVehicleGoToPointWithAvoidanceAutomobile"},
	{461, "CTaskVehiclePursue"},
	{462, "CTaskVehicleRam"},
	{463, "CTaskVehicleSpinOut"},
	{464, "CTaskVehicleApproach"},
	{465, "CTaskVehicleThreePointTurn"},
	{466, "CTaskVehicleDeadDriver"},
	{467, "CTaskVehicleCruiseNew"},
	{468, "CTaskVehicleCruiseBoat"},
	{469, "CTaskVehicleStop"},
	{470, "CTaskVehiclePullOver"},
	{471, "CTaskVehiclePassengerExit"},
	{472, "CTaskVehicleFlee"},
	{473, "CTaskVehicleFleeAirborne"},
	{474, "CTaskVehicleFleeBoat"},
	{475, "CTaskVehicleFollowRecording"},
	{476, "CTaskVehicleFollow"},
	{477, "CTaskVehicleBlock"},
	{478, "CTaskVehicleBlockCruiseInFront"},
	{479, "CTaskVehicleBlockBrakeInFront"},
	{478, "CTaskVehicleBlockBackAndForth"},
	{481, "CTaskVehicleCrash"},
	{482, "CTaskVehicleLand"},
	{483, "CTaskVehicleLandPlane"},
	{484, "CTaskVehicleHover"},
	{485, "CTaskVehicleAttack"},
	{486, "CTaskVehicleAttackTank"},
	{487, "CTaskVehicleCircle"},
	{488, "CTaskVehiclePoliceBehaviour"},
	{489, "CTaskVehiclePoliceBehaviourHelicopter"},
	{490, "CTaskVehiclePoliceBehaviourBoat"},
	{491, "CTaskVehicleEscort"},
	{492, "CTaskVehicleHeliProtect"},
	{494, "CTaskVehiclePlayerDriveAutomobile"},
	{495, "CTaskVehiclePlayerDriveBike"},
	{496, "CTaskVehiclePlayerDriveBoat"},
	{497, "CTaskVehiclePlayerDriveSubmarine"},
	{498, "CTaskVehiclePlayerDriveSubmarineCar"},
	{499, "CTaskVehiclePlayerDriveAmphibiousAutomobile"},
	{500, "CTaskVehiclePlayerDrivePlane"},
	{501, "CTaskVehiclePlayerDriveHeli"},
	{502, "CTaskVehiclePlayerDriveAutogyro"},
	{503, "CTaskVehiclePlayerDriveDiggerArm"},
	{504, "CTaskVehiclePlayerDriveTrain"},
	{505, "CTaskVehiclePlaneChase"},
	{506, "CTaskVehicleNoDriver"},
	{507, "CTaskVehicleAnimation"},
	{508, "CTaskVehicleConvertibleRoof"},
	{509, "CTaskVehicleParkNew"},
	{510, "CTaskVehicleFollowWaypointRecording"},
	{511, "CTaskVehicleGoToNavmesh"},
	{512, "CTaskVehicleReactToCopSiren"},
	{513, "CTaskVehicleGotoLongRange"},
	{514, "CTaskVehicleWait"},
	{515, "CTaskVehicleReverse"},
	{516, "CTaskVehicleBrake"},
	{517, "CTaskVehicleHandBrake"},
	{518, "CTaskVehicleTurn"},
	{519, "CTaskVehicleGoForward"},
	{520, "CTaskVehicleSwerve"},
	{521, "CTaskVehicleFlyDirection"},
	{522, "CTaskVehicleHeadonCollision"},
	{523, "CTaskVehicleBoostUseSteeringAngle"},
	{524, "CTaskVehicleShotTire"},
	{525, "CTaskVehicleBurnout"},
	{526, "CTaskVehicleRevEngine"},
	{527, "CTaskVehicleSurfaceInSubmarine"},
	{528, "CTaskVehiclePullAlongside"},
	{529, "CTaskVehicleTransformToSubmarine"},
	{530, "CTaskAnimatedFallback"},
	{531, "None"},
	};

	constexpr std::array<uint32_t, 3> crash_peds = { "slod_human"_joaat, "slod_small_quadped"_joaat, "slod_large_quadped"_joaat };

	constexpr std::array<uint32_t, 5> crash_vehicles = { "arbitergt"_joaat, "astron2"_joaat, "cyclone2"_joaat, "ignus2"_joaat, "s95"_joaat };

	constexpr std::array<uint32_t, 46> crash_objects = {
	"prop_dummy_01"_joaat,
	"prop_dummy_car"_joaat,
	"prop_dummy_light"_joaat,
	"prop_dummy_plane"_joaat,
	"prop_distantcar_night"_joaat,
	"prop_distantcar_day"_joaat,
	"hei_bh1_08_details4_em_night"_joaat,
	"dt1_18_sq_night_slod"_joaat,
	"ss1_12_night_slod"_joaat,
	-1288391198,
	"h4_prop_bush_bgnvla_med_01"_joaat,
	"h4_prop_bush_bgnvla_lrg_01"_joaat,
	"h4_prop_bush_buddleia_low_01"_joaat,
	"h4_prop_bush_ear_aa"_joaat,
	"h4_prop_bush_ear_ab"_joaat,
	"h4_prop_bush_fern_low_01"_joaat,
	"h4_prop_bush_fern_tall_cc"_joaat,
	"h4_prop_bush_mang_ad"_joaat,
	"h4_prop_bush_mang_low_aa"_joaat,
	"h4_prop_bush_mang_low_ab"_joaat,
	"h4_prop_bush_seagrape_low_01"_joaat,
	"prop_h4_ground_cover"_joaat,
	"h4_prop_weed_groundcover_01"_joaat,
	"h4_prop_grass_med_01"_joaat,
	"h4_prop_grass_tropical_lush_01"_joaat,
	"h4_prop_grass_wiregrass_01"_joaat,
	"h4_prop_weed_01_plant"_joaat,
	"h4_prop_weed_01_row"_joaat,
	"urbanweeds02_l1"_joaat,
	"proc_forest_grass01"_joaat,
	"prop_small_bushyba"_joaat,
	"v_res_d_dildo_a"_joaat,
	"v_res_d_dildo_b"_joaat,
	"v_res_d_dildo_c"_joaat,
	"v_res_d_dildo_d"_joaat,
	"v_res_d_dildo_e"_joaat,
	"v_res_d_dildo_f"_joaat,
	"v_res_skateboard"_joaat,
	"prop_battery_01"_joaat,
	"prop_barbell_01"_joaat,
	"prop_barbell_02"_joaat,
	"prop_bandsaw_01"_joaat,
	"prop_bbq_3"_joaat,
	"v_med_curtainsnewcloth2"_joaat,
	"bh1_07_flagpoles"_joaat,
	92962485
	};

	constexpr std::array<uint32_t, 36> valid_player_models = {
		"mp_m_freemode_01"_joaat,
		"mp_f_freemode_01"_joaat,
		"u_m_m_filmdirector"_joaat,
		"player_zero"_joaat,
		"player_one"_joaat,
		"player_two"_joaat,
		// peyote
		"A_C_Boar"_joaat,
		"A_C_Cat_01"_joaat,
		"A_C_Cow"_joaat,
		"A_C_Coyote"_joaat,
		"A_C_Deer"_joaat,
		"A_C_Husky"_joaat,
		"A_C_MtLion"_joaat,
		"A_C_Pig"_joaat,
		"A_C_Poodle"_joaat,
		"A_C_Pug"_joaat,
		"A_C_Rabbit_01"_joaat,
		"A_C_Retriever"_joaat,
		"A_C_Rottweiler"_joaat,
		"A_C_shepherd"_joaat,
		"A_C_Westy"_joaat,
		"A_C_Chickenhawk"_joaat,
		"A_C_Cormorant"_joaat,
		"A_C_Crow"_joaat,
		"A_C_Hen"_joaat,
		"A_C_Pigeon"_joaat,
		"A_C_Seagull"_joaat,
		"A_C_Dolphin"_joaat,
		"A_C_Fish"_joaat,
		"A_C_KillerWhale"_joaat,
		"A_C_SharkHammer"_joaat,
		"A_C_SharkTiger"_joaat,
		"A_C_Stingray"_joaat,
		"IG_Orleans"_joaat,
		"A_C_Chop"_joaat,
		"A_C_HumpBack"_joaat,
	};

	inline bool is_crash_ped(uint32_t model)
	{
		if (!model_info::is_model_of_type(model, eModelType::Ped, eModelType::OnlineOnlyPed))
			return true;
		for (auto iterator : crash_peds)
			if (iterator == model)
				return true;
		return false;
	}

	inline bool is_crash_vehicle(uint32_t model)
	{
		if (!model_info::is_model_of_type(model, eModelType::Vehicle, eModelType::Unk133))
			return true;
		for (auto iterator : crash_vehicles)
			if (iterator == model)
				return true;
		return false;
	}

	inline bool is_crash_object(uint32_t model)
	{
		if (!model_info::get_model(model))
			return false;

		if (!model_info::is_model_of_type(model, eModelType::Object, eModelType::Time, eModelType::Weapon, eModelType::Destructable, eModelType::WorldObject, eModelType::Sprinkler, eModelType::Unk65, eModelType::Plant, eModelType::LOD, eModelType::Unk132, eModelType::Building))
			return true;

		for (auto iterator : crash_objects)
			if (iterator == model)
				return true;

		return false;
	}

	inline bool is_valid_player_model(uint32_t model)
	{
		for (auto iterator : valid_player_models)
			if (iterator == model)
				return true;
		return false;
	}

	inline void check_player_model(player_ptr player, uint32_t model)
	{
		if (!player)
			return;

		if (NETWORK::NETWORK_IS_ACTIVITY_SESSION())
			return;

		if (!is_valid_player_model(model))
		{
			MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PLAYER_MODEL"), player->get_name());
			player->is_modder = true;
		}
	}

	// the game function does weird stuff that we don't want
	inline CObject* get_entity_attached_to(CObject* entity)
	{
		if (!entity)
			return nullptr;

		if (!entity->gap50)
			return nullptr;

		__int64 component = *(__int64*)((__int64)(entity->gap50) + 0x48);

		if (!component)
			return nullptr;

		int unk_count = *(int*)(component + 0x5C) & 0xF;

		if (unk_count < 2)
			return nullptr;

		return *(CObject**)(component);
	}

	inline bool is_attachment_infinite(rage::netObject* object, uint16_t attached_to_net_id, int from_bone, int to_bone)
	{
		if (object == nullptr)
			return false;

		auto target = g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr, attached_to_net_id, false);
		while (target)
		{
			if (target == object)
				return true;

			auto next = get_entity_attached_to(target->GetGameObject());

			if (!next)
				return false;

			if (!next->m_net_object)
				return false;

			target = next->m_net_object;
		}

		return false;
	}

	inline bool is_sane_override_pos(float x, float y, float z)
	{
		bool crash = (char)(fmaxf(0.0, (x + 149.0) - -8192.0) / 75.0) == 255 || (char)(fmaxf(0.0, (y + 149.0) - -8192.0) / 75.0) == 255;
		return !crash;
	}

	inline bool is_in_vehicle(CPed* ped, CVehicle* vehicle)
	{
		if (!ped || !vehicle)
			return false;

		if (ped == vehicle->m_driver)
			return true;

		for (int i = 0; i < 15; i++)
			if (vehicle->m_passengers[i] == ped)
				return true;

		return false;
	}

	inline bool is_local_player_an_occupant(CVehicleProximityMigrationDataNode* node)
	{
		for (int i = 0; i < 16; i++)
		{
			if (node->m_has_occupants[i] && node->m_occupants[i] == gta_util::get_local_ped()->m_net_object->m_object_id)
				return true;
		}

		return false;
	}

	inline std::string get_task_type_string(int type)
	{
		std::string buffer = "";

		for (auto& value : task_type_names)
		{
			if (value.first == type)
			{
				buffer += value.second;
				buffer += '|';
			}

			if (value.first > type)
				break; // a minor optimization
		}

		if (buffer.length() > 1)
			buffer.pop_back();
		else
			buffer = "Unknown";

		buffer += " (" + std::to_string(type) + ")";

		return buffer;
	}

	inline bool is_valid_clan_tag(char* data, bool system_clan)
	{
		int length = strlen(data);

		if (length <= (system_clan ? 2 : 3))
			return false;

		for (int i = 0; i < length; i++)
		{
			if (data[i] >= '0' && data[i] <= '9')
				continue;

			if (data[i] >= 'A' && data[i] <= 'Z')
				continue;

			if (data[i] >= 'a' && data[i] <= 'z')
				continue;

			return false;
		}

		return true;
	}

	bool check_node(rage::netSyncNodeBase* node, CNetGamePlayer* sender, rage::netObject* object)
	{
		if (node->IsParentNode())
		{
			for (auto child = node->m_first_child; child; child = child->m_next_sibling)
			{
				if (check_node(child, sender, object))
					return true;
			}
		}
		else if (node->IsDataNode())
		{
			const auto addr = (uintptr_t)node;
			auto sender_plyr = g_player_service->get_by_id(sender->m_player_id);
			const auto& node_id = sync_node_finder::find((eNetObjType)object->m_object_type, addr);

			if ((((CProjectBaseSyncDataNode*)node)->flags & 1) == 0)
				return false;

			switch (node_id)
			{
			case sync_node_id("CVehicleCreationDataNode"):
			{
				const auto creation_node = (CVehicleCreationDataNode*)(node);
				if (is_crash_vehicle(creation_node->m_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_VEHICLE_MODEL"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CDoorCreationDataNode"):
			{
				if (const auto creation_node = (CDoorCreationDataNode*)(node); is_crash_object(creation_node->m_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_DOOR_MODEL"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CPickupCreationDataNode"):
			{
				if (const auto creation_node = (CPickupCreationDataNode*)(node); creation_node->m_custom_model && is_crash_object(creation_node->m_custom_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PICKUP_MODEL"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CPhysicalAttachDataNode"):
			{
				const auto attach_node = (CPhysicalAttachDataNode*)(node);

				if (attach_node->m_attached
					&& is_attachment_infinite(object,
						attach_node->m_attached_to,
						attach_node->m_attach_bone,
						attach_node->m_other_attach_bone))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INFINITE_PHYSICAL_ATTACHMENT"), sender->GetName());
					return true;
				}

				if (attach_node->m_attached && object && object->m_object_type == (int16_t)eNetObjType::NET_OBJ_TYPE_TRAILER)
				{
					if (auto net_obj =
						g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr, attach_node->m_attached_to, false))
					{
						if (auto entity = net_obj->GetGameObject())
						{
							if (entity->m_entity_type != 3)
							{
								MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PHYSICAL_ATTACHMENT"), sender->GetName());
								return true;
							}
						}
					}
				}
				break;
			}
			case sync_node_id("CPedCreationDataNode"):
			{
				if (const auto creation_node = (CPedCreationDataNode*)(node); is_crash_ped(creation_node->m_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PED_MODEL"), sender->GetName());
					return true;
				}
				else if (creation_node->m_has_prop && is_crash_object(creation_node->m_prop_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PED_PROP_MODEL"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CPedAttachDataNode"):
			{
				const auto attach_node = (CPedAttachDataNode*)(node);
				if (attach_node->m_attached
					&& is_attachment_infinite(object,
						attach_node->m_attached_to,
						attach_node->m_attachment_bone,
						attach_node->m_attachment_bone))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INFINITE_PED_ATTACHMENT"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CObjectCreationDataNode"):
			{
				if (const auto creation_node = (CObjectCreationDataNode*)(node); is_crash_object(creation_node->m_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_OBJECT_MODEL"), sender->GetName());
					return true;
				}
				break;
			}
			case sync_node_id("CPlayerAppearanceDataNode"):
			{
				const auto player_appearance_node = (CPlayerAppearanceDataNode*)(node);
				if (is_crash_ped(player_appearance_node->m_model_hash))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PLAYER_MODEL_AN"), sender->GetName());
					return true;
				}
				check_player_model(sender_plyr, player_appearance_node->m_model_hash);
				break;
			}
			case sync_node_id("CPlayerCreationDataNode"):
			{
				const auto player_creation_node = (CPlayerCreationDataNode*)(node);
				if (is_crash_ped(player_creation_node->m_model))
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_PLAYER_MODEL_CN"), sender->GetName());
					return true;
				}
				check_player_model(sender_plyr, player_creation_node->m_model);
				break;
			}
			case sync_node_id("CSectorDataNode"):
			{
				if (const auto sector_node = (CSectorDataNode*)(node); sector_node->m_pos_x == 712 || sector_node->m_pos_y == 712 || sector_node->m_pos_z == 712)
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_SECTOR_POSITION"), sender->GetName(), sector_node->m_pos_x, sector_node->m_pos_y, sector_node->m_pos_z);
					return true;
				}
				break;
			}
			case sync_node_id("CPlayerGameStateDataNode"):
			{
				const auto game_state_node = (CPlayerGameStateDataNode*)(node);
				if (game_state_node->m_is_overriding_population_control_sphere
					&& !is_sane_override_pos(game_state_node->m_population_control_sphere_x, game_state_node->m_population_control_sphere_y, game_state_node->m_population_control_sphere_z)
					&& gta_util::get_network()->m_game_session_ptr->is_host())
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("INVALID_SECTOR_PLAYER_GAME_STATE"), sender->GetName());
					return true;
				}
				if (sender_plyr)
				{
					if (game_state_node->m_is_spectating)
					{
						if (!sender_plyr->get_ped())
							break;

						if (sender_plyr->get_ped()->m_health <= 0.0f) // you spectate the player that killed you
							break;

						auto net_obj = g_pointers->m_get_net_object(*g_pointers->m_network_object_mgr,
							game_state_node->m_spectating_net_id,
							false);

						if (!net_obj)
							break;

						auto entity = net_obj->GetGameObject();

						if (!entity || entity->m_entity_type != 4)
							break;

						auto player_info = ((CPed*)entity)->m_player_info;

						if (!player_info)
							break;

						player_ptr target = nullptr;

						if (gta_util::get_local_ped() && (CPed*)entity == gta_util::get_local_ped())
						{
							target = g_player_service->get_self();
						}
						else
						{
							for (auto p : g_player_service->players())
								if (p.second->get_player_info() == player_info)
									target = p.second;
						}

						if (!target || !target->is_valid())
							break;

						if (target->id() != sender_plyr->spectating_player)
						{
							if (target->id() == PLAYER::PLAYER_ID())
								MainNotification(ImGuiToastType_Info, 10000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("PLAYER_IS_SPECTATING_ME"), sender_plyr->get_name());
							else
								MainNotification(ImGuiToastType_Info, 10000, TRANSLATE("NETWORK_OPTIONS"), TRANSLATE("PLAYER_IS_SPECTATING_PLAYER"), sender_plyr->get_name(), target->get_name());

							sender_plyr->spectating_player = target->id();
						}
					}
					else
					{
						sender_plyr->spectating_player = -1;
					}
				}
				break;
			}
			case sync_node_id("CPlayerGamerDataNode"):
			{
				const auto gamer_node = (CPlayerGamerDataNode*)(node);

				if (sender_plyr)
				{
					if (gamer_node->m_clan_data.m_clan_id == 123456 && gamer_node->m_clan_data.m_clan_id_2 == 123456)
					{
						sender_plyr->is_modder = true;
						MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("SPOOFED_DATA"), sender->GetName());
					}
					else if (gamer_node->m_clan_data.m_clan_id > 0 && gamer_node->m_clan_data.m_clan_id_2 > 0)
					{
						if (!is_valid_clan_tag(gamer_node->m_clan_data.m_clan_tag, gamer_node->m_clan_data.m_is_system_clan))
						{
							sender_plyr->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("SPOOFED_DATA"), sender->GetName());
						}

						if (gamer_node->m_clan_data.m_is_system_clan
							&& (!gamer_node->m_clan_data.m_is_clan_open || gamer_node->m_clan_data.m_clan_member_count == 0))
						{
							sender_plyr->is_modder = true;
							MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("SPOOFED_DATA"), sender->GetName());
						}
					}
				}
				break;
			}
			case sync_node_id("CPedGameStateDataNode"):
			{
				const auto game_state_node = (CPedGameStateDataNode*)(node);
				if (game_state_node->m_on_mount)
				{
					//notify::crash_blocked(sender, "mount flag");
					return true;
				}
				for (int i = 0; i < game_state_node->m_num_equiped_gadgets; i++)
				{
					if (game_state_node->m_gadget_hash[i] != "gadget_parachute"_joaat && game_state_node->m_gadget_hash[i] != "gadget_nightvision"_joaat)
					{
						//notify::crash_blocked(sender, "invalid gadget");
						return true;
					}
				}
				break;
			}
			case sync_node_id("CTrainGameStateDataNode"):
			{
				if (const auto train_node = (CTrainGameStateDataNode*)(node); train_node->m_track_id < 0 || train_node->m_track_id >= 27)
				{
					MainNotification(ImGuiToastType_Protections, 7000, TRANSLATE("PROTECTIONS"), TRANSLATE("OUT_OF_BOUNDS_TRAIN_TRACK_INDEX"), sender->GetName(), train_node->m_track_id);
					return true;
				}
				break;
			}
			//case sync_node_id("CVehicleControlDataNode"):
			//{
			//	const auto control_node = (CVehicleControlDataNode*)(node);
			//	if (control_node->m_is_submarine_car)
			//	{
			//		if (auto vehicle = (CVehicle*)object->GetGameObject())
			//		{
			//			if (auto model_info = vehicle->m_model_info)
			//			{
			//				if (model_info->m_hash != "stromberg"_joaat && model_info->m_hash != "toreador"_joaat)
			//				{
			//					//notify::crash_blocked(sender, "submarine car");
			//					return true;
			//				}
			//			}
			//		}
			//	}

			//	break;
			//}
			case sync_node_id("CVehicleProximityMigrationDataNode"):
			{
				if (object && gta_util::get_local_ped() && gta_util::get_local_ped()->m_net_object)
				{
					const auto migration_node = (CVehicleProximityMigrationDataNode*)(node);

					if (!gta_util::get_local_ped()->m_vehicle || !gta_util::get_local_ped()->m_vehicle->m_net_object
						|| gta_util::get_local_ped()->m_vehicle->m_net_object->m_object_id != object->m_object_id
						|| !is_in_vehicle(gta_util::get_local_ped(), gta_util::get_local_ped()->m_vehicle))
					{
						if (is_local_player_an_occupant(migration_node))
						{
							return true; // remote teleport
						}
					}
				}

				break;
			}
			}
		}
		return false;
	}

	bool hooks::can_apply_data(rage::netSyncTree* tree, rage::netObject* object) {
		static bool init = ([] { sync_node_finder::init(); }(), true);
		if (tree->m_child_node_count && tree->m_next_sync_node && check_node(tree->m_next_sync_node, m_syncing_player, object)) {
			return false;
		}
		return g_hooking->m_canApplyDataHk.getOg<decltype(&can_apply_data)>()(tree, object);
	}
}