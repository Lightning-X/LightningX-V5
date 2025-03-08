#pragma once
#include "../Common.hpp"
#include "GTAV-Classes-master/ped/CPedFactory.hpp"
#include "GTAV-Classes-master/network/netObject.hpp"

class CPickup {
public:
	char pad_0x0000[0x30]; //0x0000
	class CNavigation* m_navigation; //0x0030
	char pad_0x0038[0x58]; //0x0083
	rage::fvector3 m_position; //0x0090
	char pad_0x009C[1012]; //0x009C
	int32_t m_money; //0x0490
	char pad_0x0484[0x104]; //0x0484
};//Size=0x0588
class CPickupHandle {
public:
	CPickup* m_pickup; //0x0000
	int32_t m_handle; //0x0008
	char pad_0x000C[4]; //0x000C
}; //Size=0x0010
//Max pickups: 73
class CPickupList {
public:
	CPickupHandle m_pickups[73]; //0x0000
	uintptr_t getAddress(int index) {
		uintptr_t addr = (uintptr_t)static_cast<void*>(this) + (index * 0x10);
		return *(uintptr_t*)addr;
	}
}; //Size=0x0490
class CPickupInterface {
public:
	char pad_0x0000[0x100]; //0x0000
	CPickupList* m_pickup_list; //0x0100
	int32_t m_max_pickups; //0x0108
	int32_t m_cur_pickups; //0x0110
	CPickup* get_pickup(const int& index) {
		if (index < m_max_pickups)
			return m_pickup_list->m_pickups[index].m_pickup;
		return nullptr;
	}
}; //Size=0x0114
class CObjectHandle {
public:
	CObject* m_object; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CObjectHandle) == 0x10);
class CObjectList {
public:
	CObjectHandle m_objects[2300]; //0x0000
	uintptr_t getAddress(int index) {
		uintptr_t addr = (uintptr_t)static_cast<void*>(this) + (index * 0x10);
		return *(uintptr_t*)addr;
	}
}; //Size: 0x8FC0
class CObjectInterface {
public:
	char pad_0000[344]; //0x0000
	CObjectList* m_object_list; //0x0158
	int32_t m_max_objects; //0x0160
	char pad_0164[4]; //0x0164
	int32_t m_cur_objects; //0x0168
	CObject* get_object(const int& index) {
		if (index < m_max_objects)
			return m_object_list->m_objects[index].m_object;
		return nullptr;
	}
}; //Size: 0x016C
class CPedHandle {
public:
	CPed* m_ped; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CPedHandle) == 0x10, "CPedHandle is not properly sized");
class CPedList {
public:
	CPedHandle m_peds[256]; //0x0000
	uintptr_t getAddress(int index) {
		uintptr_t addr = (uintptr_t)static_cast<void*>(this) + (index * 0x10);
		return *(uintptr_t*)addr;
	}
}; //Size: 0x1000
class CPedInterface {
public:
	char pad_0000[256]; //0x0000
	CPedList* m_ped_list; //0x0100
	int32_t m_max_peds; //0x0108
	char pad_010C[4]; //0x010C
	int32_t m_cur_peds; //0x0110
	CPed* get_ped(const int& index) {
		if (index < m_max_peds)
			return m_ped_list->m_peds[index].m_ped;
		return nullptr;
	}
}; //Size: 0x0114
class CVehicleHandle {
public:
	class CVehicle* m_vehicle; //0x0000
	int32_t m_handle; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010
static_assert(sizeof(CVehicleHandle) == 0x10, "CVehicleHandle is not properly sized");
class CVehicleList {
public:
	CVehicleHandle m_vehicles[300]; //0x0000
	uintptr_t getAddress(int index) {
		uintptr_t addr = (uintptr_t)static_cast<void*>(this) + (index * 0x10);
		return *(uintptr_t*)addr;
	}
}; //Size: 0x12C0
class CVehicleInterface {
public:
	char pad_0000[384]; //0x0000
	CVehicleList* m_vehicle_list; //0x0180
	int32_t m_max_vehicles; //0x0188
	char pad_018C[4]; //0x018C
	int32_t m_cur_vehicles; //0x0190
	CVehicle* get_vehicle(const int& index) {
		if (index < m_max_vehicles)
			return m_vehicle_list->m_vehicles[index].m_vehicle;
		return nullptr;
	}
}; //Size: 0x0194
class CCameraInterface {
public:
	char pad_0x0000[0x408]; //0x0000
}; //Size=0x0408
class CReplayInterface {
public:
	char pad_0x0000[0x8]; //0x0000
	CCameraInterface* m_camera_interface; //0x0008
	CVehicleInterface* m_vehicle_interface; //0x0010
	CPedInterface* m_ped_interface; //0x0018
	CPickupInterface* m_pickup_interface; //0x0020
	CObjectInterface* m_object_interface; //0x0028
	char pad_0x0030[0x10]; //0x0030
}; //Size=0x0040