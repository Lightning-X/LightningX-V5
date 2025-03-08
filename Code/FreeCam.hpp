#pragma once
namespace base::features {
	static bool bLastFreeCam = false;
	static float Freecamspeed = 0.5f;
	static float Freecammult = 0.f;
	static Cam cCam = -1;
	static Vector3 vecPosition;
	static Vector3 vecRot;
	static const ControllerInputs Freecamcontrols[] = { ControllerInputs::INPUT_LOOK_LR, ControllerInputs::INPUT_LOOK_UD, ControllerInputs::INPUT_LOOK_UP_ONLY, ControllerInputs::INPUT_LOOK_DOWN_ONLY, ControllerInputs::INPUT_LOOK_LEFT_ONLY, ControllerInputs::INPUT_LOOK_RIGHT_ONLY, ControllerInputs::INPUT_LOOK_LEFT, ControllerInputs::INPUT_LOOK_RIGHT, ControllerInputs::INPUT_LOOK_UP, ControllerInputs::INPUT_LOOK_DOWN };
	void self_free_cam() {
		bool bFreeCam = Freecambool;
		if (bFreeCam || (!bFreeCam && bFreeCam != bLastFreeCam)) {
			if (!CAM::DOES_CAM_EXIST(cCam)) {
				cCam = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 0);
				vecPosition = CAM::GET_GAMEPLAY_CAM_COORD();
				vecRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
				ENTITY::FREEZE_ENTITY_POSITION(PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID()), true);
				CAM::SET_CAM_COORD(cCam, vecPosition);
				CAM::SET_CAM_ROT(cCam, vecRot, 2);
				CAM::SET_CAM_ACTIVE(cCam, true);
				CAM::RENDER_SCRIPT_CAMS(true, true, 500, true, true, 0);
			}
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			for (const auto& control : Freecamcontrols)
				PAD::ENABLE_CONTROL_ACTION(0, static_cast<int>(control), true);
			Vector3 vecChange = { 0.f, 0.f, 0.f };
			// Left Shift
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_SPRINT))
				vecChange.z += Freecamspeed / 2;
			// Left Control
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_DUCK))
				vecChange.z -= Freecamspeed / 2;
			// Forward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_UP_ONLY))
				vecChange.y += Freecamspeed;
			// Backward
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_DOWN_ONLY))
				vecChange.y -= Freecamspeed;
			// Left
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_LEFT_ONLY))
				vecChange.x -= Freecamspeed;
			// Right
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, (int)ControllerInputs::INPUT_MOVE_RIGHT_ONLY))
				vecChange.x += Freecamspeed;

			if (vecChange.x == 0.f && vecChange.y == 0.f && vecChange.z == 0.f)
				Freecammult = 0.f;
			else if (Freecammult < 10)
				Freecammult += 0.15f;
			Vector3 rot = CAM::GET_CAM_ROT(cCam, 2);
			//float pitch = math::deg_to_rad(rot.x); // vertical
			//float roll = rot.y;
			float yaw = math::deg_to_rad(rot.z);// horizontal
			vecPosition.x += (vecChange.x * cos(yaw) - vecChange.y * sin(yaw)) * Freecammult;
			vecPosition.y += (vecChange.x * sin(yaw) + vecChange.y * cos(yaw)) * Freecammult;
			vecPosition.z += vecChange.z * Freecammult;
			CAM::SET_CAM_COORD(cCam, vecPosition);
			STREAMING::SET_FOCUS_POS_AND_VEL(vecPosition, Vector3(0.f, 0.f, 0.f));
			vecRot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			CAM::SET_CAM_ROT(cCam, vecRot, 2);
			if (!Freecambool && CAM::DOES_CAM_EXIST(cCam)) {
				CAM::SET_CAM_ACTIVE(cCam, false);
				CAM::RENDER_SCRIPT_CAMS(false, true, 500, true, true, 0);
				CAM::DESTROY_CAM(cCam, false);
				STREAMING::CLEAR_FOCUS();
				ENTITY::FREEZE_ENTITY_POSITION(cCam, false);
			}
			bLastFreeCam = Freecambool;
		}
	}
}