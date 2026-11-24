/* SPDX-License-Identifier: MIT */

#define DT_DRV_COMPAT mona_behavior_host_key

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <drivers/behavior.h>
#include <zmk/endpoints.h>
#include <zmk/events/keycode_state_changed.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

/* Positions include Studio macro triggers and virtual combo positions. */
struct host_key_press {
    bool in_use;
    uint32_t position;
    uint8_t source;
    uint32_t primary;
    uint32_t apple;
    uint32_t keycode;
};

static struct host_key_press presses[32];
static struct k_spinlock press_lock;

static int host_key_event(const struct zmk_behavior_binding *binding,
                          struct zmk_behavior_binding_event event, bool pressed) {
    struct zmk_endpoint_instance endpoint = zmk_endpoint_get_selected();
    // User slots 2–4 correspond to zero-based BLE indices 1–3. USB is primary.
    bool use_apple = endpoint.transport == ZMK_TRANSPORT_BLE &&
                     endpoint.ble.profile_index >= 1 && endpoint.ble.profile_index <= 3;

    k_spinlock_key_t lock_key = k_spin_lock(&press_lock);
    struct host_key_press *free_slot = NULL;
    for (size_t i = 0; i < ARRAY_SIZE(presses); i++) {
        struct host_key_press *entry = &presses[i];
        if (!entry->in_use) {
            if (!free_slot) {
                free_slot = entry;
            }
            continue;
        }
        if (entry->position != event.position || entry->source != event.source ||
            entry->primary != binding->param1 || entry->apple != binding->param2) {
            continue;
        }
        if (pressed) {
            k_spin_unlock(&press_lock, lock_key);
            return ZMK_BEHAVIOR_OPAQUE;
        }
        // Release the same key even if the host changed since the press.
        uint32_t keycode = entry->keycode;
        entry->in_use = false;
        k_spin_unlock(&press_lock, lock_key);
        return raise_zmk_keycode_state_changed_from_encoded(keycode, false, event.timestamp);
    }

    if (!pressed || !free_slot) {
        k_spin_unlock(&press_lock, lock_key);
        return pressed ? -ENOMEM : ZMK_BEHAVIOR_OPAQUE;
    }

    uint32_t keycode = use_apple ? binding->param2 : binding->param1;
    *free_slot = (struct host_key_press){
        .in_use = true,
        .position = event.position,
        .source = event.source,
        .primary = binding->param1,
        .apple = binding->param2,
        .keycode = keycode,
    };
    k_spin_unlock(&press_lock, lock_key);
    return raise_zmk_keycode_state_changed_from_encoded(keycode, true, event.timestamp);
}

static int host_key_pressed(struct zmk_behavior_binding *binding,
                            struct zmk_behavior_binding_event event) {
    return host_key_event(binding, event, true);
}

static int host_key_released(struct zmk_behavior_binding *binding,
                             struct zmk_behavior_binding_event event) {
    return host_key_event(binding, event, false);
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
static const struct behavior_parameter_value_metadata primary_values[] = {{
    .display_name = "Windows / USB / BT 5",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE,
}};
static const struct behavior_parameter_value_metadata apple_values[] = {{
    .display_name = "Apple / BT 2–4",
    .type = BEHAVIOR_PARAMETER_VALUE_TYPE_HID_USAGE,
}};
static const struct behavior_parameter_metadata_set metadata_sets[] = {{
    .param1_values = primary_values,
    .param1_values_len = ARRAY_SIZE(primary_values),
    .param2_values = apple_values,
    .param2_values_len = ARRAY_SIZE(apple_values),
}};
static const struct behavior_parameter_metadata metadata = {
    .sets = metadata_sets,
    .sets_len = ARRAY_SIZE(metadata_sets),
};
#endif

static const struct behavior_driver_api host_key_api = {
    .binding_pressed = host_key_pressed,
    .binding_released = host_key_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif
};

#define HOST_KEY_INST(n)                                                                            \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                 \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &host_key_api);

DT_INST_FOREACH_STATUS_OKAY(HOST_KEY_INST)

#endif
