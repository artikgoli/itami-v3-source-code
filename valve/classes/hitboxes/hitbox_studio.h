#pragma once

enum bones_t
{
    BONE_INVALID = -1,

    BONE_PELVIS = 0,
    BONE_SPINE_0 = 1,
    BONE_SPINE_1 = 2,
    BONE_SPINE_2 = 3,
    BONE_SPINE_3 = 4,
    BONE_NECK = 5,
    BONE_HEAD = 6,

    BONE_LEFT_SHOULDER = 8,
    BONE_LEFT_ARM = 9,
    BONE_LEFT_HAND = 10,

    BONE_RIGHT_SHOULDER = 13,
    BONE_RIGHT_ARM = 14,
    BONE_RIGHT_HAND = 15,

    BONE_LEFT_HIP = 22,
    BONE_LEFT_KNEE = 23,
    BONE_LEFT_FOOT = 24,

    BONE_RIGHT_HIP = 25,
    BONE_RIGHT_KNEE = 26,
    BONE_RIGHT_FOOT = 27,

    BONE_HIP = BONE_PELVIS,
    BONE_SPINE = BONE_SPINE_3,
    BONE_SPINE_1_OLD = BONE_SPINE_1,
    BONE_LEFT_FEET = BONE_LEFT_FOOT,
    BONE_RIGHT_FEET = BONE_RIGHT_FOOT
};

enum hitboxes_t : int
{
    HITBOX_INVALID = -1,
    HITBOX_HEAD,
    HITBOX_NECK,
    HITBOX_PELVIS,
    HITBOX_STOMACH,
    HITBOX_THORAX,
    HITBOX_CHEST,
    HITBOX_UPPER_CHEST,
    HITBOX_LEFT_THIGH,
    HITBOX_RIGHT_THIGH,
    HITBOX_LEFT_CALF,
    HITBOX_RIGHT_CALF,
    HITBOX_LEFT_FOOT,
    HITBOX_RIGHT_FOOT,
    HITBOX_LEFT_HAND,
    HITBOX_RIGHT_HAND,
    HITBOX_LEFT_UPPER_ARM,
    HITBOX_LEFT_FOREARM,
    HITBOX_RIGHT_UPPER_ARM,
    HITBOX_RIGHT_FOREARM,
    HITBOX_MAX
};

class c_hitbox {
public:
    vector3d get_mins() {
        return memory->read<vector3d>(reinterpret_cast<uintptr_t>(this) + 0x18);
    }
    vector3d get_maxs() {
        return memory->read<vector3d>(reinterpret_cast<uintptr_t>(this) + 0x24);
    }
    float get_radius() {
        return memory->read<float>(reinterpret_cast<uintptr_t>(this) + 0x30);
    }
    int get_group_index() {
        return memory->read<int>(reinterpret_cast<uintptr_t>(this) + 0x38);
    }
    std::string get_hitbox_name() {
        uintptr_t namePtr = get_name_pointer();
        if (!namePtr) return "";
        return memory->read_string(namePtr);
    }

    static int get_bone_from_hitbox(int hitbox_index)
    {
        static constexpr std::array<int, HITBOX_MAX> hitbox_to_bone_map = {
            bones_t::BONE_HEAD, bones_t::BONE_NECK, bones_t::BONE_PELVIS, bones_t::BONE_SPINE_0,
            bones_t::BONE_SPINE_1, bones_t::BONE_SPINE_2, bones_t::BONE_SPINE_3, bones_t::BONE_LEFT_HIP,
            bones_t::BONE_RIGHT_HIP, bones_t::BONE_LEFT_KNEE, bones_t::BONE_RIGHT_KNEE, bones_t::BONE_LEFT_FEET,
            bones_t::BONE_RIGHT_FEET, bones_t::BONE_LEFT_HAND, bones_t::BONE_RIGHT_HAND, bones_t::BONE_LEFT_SHOULDER,
            bones_t::BONE_LEFT_ARM, bones_t::BONE_RIGHT_SHOULDER, bones_t::BONE_RIGHT_ARM
        };

        if (hitbox_index < 0 || hitbox_index >= static_cast<int>(hitbox_to_bone_map.size()))
            return -1;

        return hitbox_to_bone_map[hitbox_index];
    }

private:
    uintptr_t get_name_pointer() {
        return memory->read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x0);
    }
};


class c_hitbox_set {
public:
    int get_hitbox_count() {
        return memory->read<int>(reinterpret_cast<uintptr_t>(this) + 0x38);
    }

    uintptr_t get_hitbox_array() {
        return memory->read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + 0x30);
    }
};

class c_render_mesh {
public:
    c_hitbox_set* get_hitbox_set() {
        return memory->read<c_hitbox_set*>(reinterpret_cast<uintptr_t>(this) + 0x150);
    }
};
