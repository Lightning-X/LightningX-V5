#pragma once

namespace base::features {
	static bool bLastNoClip = false;
	static constexpr ControllerInputs controls[] = {
		ControllerInputs::INPUT_SPRINT,
		ControllerInputs::INPUT_MOVE_UP_ONLY,
		ControllerInputs::INPUT_MOVE_DOWN_ONLY,
		ControllerInputs::INPUT_MOVE_LEFT_ONLY,
		ControllerInputs::INPUT_MOVE_RIGHT_ONLY,
		ControllerInputs::INPUT_DUCK
	};
	static constexpr float speed = 1.0f;

	float m_speed_multiplier{};

	Entity m_entity{};

	void NoClip() {
		bool bNoClip = NoClipBool;

		if (bNoClip || (!bNoClip && bNoClip != bLastNoClip))
		{
			for (const auto& control : controls)
				PAD::DISABLE_CONTROL_ACTION(0, static_cast<int>(control), true);

			float noclip_aim_speed_multiplier = 0.25f;
			float noclip_speed_multiplier = 20.f;

			const auto ped = PLAYER::PLAYER_PED_ID();
			const auto veh = PED::GET_VEHICLE_PED_IS_IN(ped, FALSE);
			const auto location = ENTITY::GET_ENTITY_COORDS(ped, TRUE);
			const Entity ent = (veh != 0 && gta_util::get_local_ped()->m_ped_task_flag & (int)ePedTask::TASK_DRIVING) ? veh : ped;

			while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_sum2_hal")) {
				STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_sum2_hal");
				script::get_current()->yield(5ms);
			}
			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_sum2_hal");
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("sp_sum2_hal_rider_death_trail_blue", ped, Vector3(0.1f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), (int)PedBones::SKEL_L_Hand, 0.7f, false, false, false);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_COLOUR(255, 255, 255);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_ALPHA(1.f);

			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_sum2_hal");
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("sp_sum2_hal_rider_death_trail_blue", ped, Vector3(0.1f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), (int)PedBones::SKEL_R_Hand, 0.7f, false, false, false);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_COLOUR(255, 255, 255);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_ALPHA(1.f);

			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_sum2_hal");
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("sp_sum2_hal_rider_death_trail_blue", ped, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), (int)PedBones::SKEL_L_Foot, 0.7f, false, false, false);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_COLOUR(255, 255, 255);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_ALPHA(1.f);

			GRAPHICS::USE_PARTICLE_FX_ASSET("scr_sum2_hal");
			GRAPHICS::START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE("sp_sum2_hal_rider_death_trail_blue", ped, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), (int)PedBones::SKEL_R_Foot, 0.7f, false, false, false);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_COLOUR(255, 255, 255);
			GRAPHICS::SET_PARTICLE_FX_NON_LOOPED_ALPHA(1.f);

			// cleanup when changing entities
			if (m_entity != ent)
			{
				ENTITY::FREEZE_ENTITY_POSITION(m_entity, FALSE);
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(m_entity, TRUE, FALSE);
				ENTITY::RESET_ENTITY_ALPHA(m_entity);

				m_entity = ent;
			}

			Vector3 vel{};

			// Left Shift
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_SPRINT))
				vel.z += speed / 2;
			// Left Control
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_DUCK))
				vel.z -= speed / 2;
			// Forward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_UP_ONLY))
				vel.y += speed;
			// Backward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_DOWN_ONLY))
				vel.y -= speed;
			// Left
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_LEFT_ONLY))
				vel.x -= speed;
			// Right
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_RIGHT_ONLY))
				vel.x += speed;

			auto rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			ENTITY::SET_ENTITY_ROTATION(ent, 0.f, rot.y, rot.z, 2, 0);
			ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(ent, FALSE, FALSE);
			ENTITY::SET_ENTITY_ALPHA(ent, 160, FALSE);
			if (vel.x == 0.f && vel.y == 0.f && vel.z == 0.f)
			{
				// freeze entity to prevent drifting when standing still
				ENTITY::FREEZE_ENTITY_POSITION(ent, TRUE);
				m_speed_multiplier = 0.f;
			}
			else
			{
				if (m_speed_multiplier < 20.f)
					m_speed_multiplier += 0.15f;

				ENTITY::FREEZE_ENTITY_POSITION(ent, FALSE);

				const auto is_aiming = PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_AIM);
				if (is_aiming)
				{
					vel = vel * noclip_aim_speed_multiplier;

					const auto offset = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ent, Vector3(vel.x * m_speed_multiplier, vel.y * m_speed_multiplier, vel.z * m_speed_multiplier));

					ENTITY::SET_ENTITY_VELOCITY(ent, vel);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, offset, TRUE, TRUE, TRUE);
				}
				else
				{
					vel = vel * noclip_speed_multiplier;

					const auto offset = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(ent, Vector3(vel.x, vel.y, 0.f));
					vel.x = offset.x - location.x;
					vel.y = offset.y - location.y;

					ENTITY::SET_ENTITY_MAX_SPEED(ent, 999999999999);
					ENTITY::SET_ENTITY_VELOCITY(ent, Vector3(vel.x * m_speed_multiplier, vel.y * m_speed_multiplier, vel.z * m_speed_multiplier));
				}
			}

			if (!bNoClip && take_control_of(m_entity))
			{
				ENTITY::RESET_ENTITY_ALPHA(m_entity);
				ENTITY::FREEZE_ENTITY_POSITION(m_entity, FALSE);
				ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(m_entity, TRUE, FALSE);
				STREAMING::REMOVE_NAMED_PTFX_ASSET("scr_sum2_hal");
				STREAMING::REMOVE_PTFX_ASSET();
			}

			bLastNoClip = NoClipBool;
		}
	}
}