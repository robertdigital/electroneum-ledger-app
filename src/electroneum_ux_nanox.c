// Copyright (c) Electroneum Limited 2019-2020
/* Copyright 2017 Cedric Mesnil <cslashm@gmail.com>, Ledger SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



#ifdef UI_NANO_X

#include "os.h"
#include "cx.h"
#include "electroneum_types.h"
#include "electroneum_api.h"
#include "electroneum_vars.h"

#include "electroneum_ux_msg.h"
#include "os_io_seproxyhal.h"
#include "string.h"
#include "glyphs.h"

/* ----------------------------------------------------------------------- */
/* ---                        NanoS  UI layout                         --- */
/* ----------------------------------------------------------------------- */


#define ACCEPT  0xACCE
#define REJECT  ~ACCEPT


void ui_menu_main_display(unsigned int value);

/* -------------------------------------- 25 WORDS --------------------------------------- */
void ui_menu_words_display(unsigned int value);
void ui_menu_words_clear(unsigned int value);
void ui_menu_words_back(unsigned int value);


UX_STEP_NOCB(
  ux_menu_words_1_step,
  bnnn_paging,
  {
    .title = "Electrum Seed",
    .text = G_electroneum_vstate.ux_words,
  });


UX_STEP_CB(
  ux_menu_words_2_step,
  bn,
  ui_menu_words_clear(0),
  {
    "CLEAR WORDS",
    "(Do not wipe the wallet)"
  });

UX_STEP_CB(
  ux_menu_words_3_step,
  pb,
  ui_menu_words_back(0),
  {
    &C_icon_back,
    "back"
  });

UX_FLOW(
  ux_flow_words,
  &ux_menu_words_1_step,
  &ux_menu_words_2_step,
  &ux_menu_words_3_step
  );

void ui_menu_words_clear(unsigned int value) {
  electroneum_clear_words();
  ui_menu_main_display(0);
}

void ui_menu_words_back(unsigned int value) {
  ui_menu_main_display(1);
}

static int ui_strncpy(char *dest, const char *src, int n) {
  int i;
  i = 0;
  while(i < n) {
    if (!src[i]) {
      break;
    }
    dest[i] = src[i];
    i++;
  }
  return i;
}

void ui_menu_words_display(unsigned int value) {
  int offset = 0;
  os_memset(G_electroneum_vstate.ux_words, 0, sizeof(G_electroneum_vstate.ux_words));
  for (int i = 0; i < 25; i++) {
    offset += ui_strncpy(G_electroneum_vstate.ux_words+offset, (char*)N_electroneum_pstate->words[i], 20);
    G_electroneum_vstate.ux_words[offset] = ' ';
    offset++;
  }
  ux_flow_init(0, ux_flow_words, NULL);
}

void settings_show_25_words(void) {
  ui_menu_words_display(0);
}

/* ----------------------------- FEE VALIDATION ----------------------------- */

void ui_menu_amount_validation_action(unsigned int value);

UX_STEP_NOCB(
  ux_menu_validation_fee_1_step,
  bn,
  {
    "Fee",
    G_electroneum_vstate.ux_amount,
  });

UX_STEP_NOCB(
  ux_menu_validation_change_1_step,
  bn,
  {
    "Change",
    G_electroneum_vstate.ux_amount,
  });

UX_STEP_CB(
  ux_menu_validation_cf_2_step,
  pb,
  ui_menu_amount_validation_action(ACCEPT),
  {
    &C_icon_validate_14,
    "Accept",
  });

UX_STEP_CB(
  ux_menu_validation_cf_3_step,
  pb,
  ui_menu_amount_validation_action(REJECT),
  {
    &C_icon_crossmark,
    "Reject",
  });

UX_FLOW(ux_flow_fee,
  &ux_menu_validation_fee_1_step,
  &ux_menu_validation_cf_2_step,
  &ux_menu_validation_cf_3_step
  );

UX_FLOW(ux_flow_change,
  &ux_menu_validation_change_1_step,
  &ux_menu_validation_cf_2_step,
  &ux_menu_validation_cf_3_step
  );


void ui_menu_amount_validation_action(unsigned int value) {
  unsigned short sw;
  if (value == ACCEPT) {
    sw = 0x9000;
  } else {
   sw = SW_SECURITY_STATUS_NOT_SATISFIED;
    electroneum_abort_tx();
  }
  electroneum_io_insert_u16(sw);
  electroneum_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
}

void ui_menu_fee_validation_display(unsigned int value) {
  ux_flow_init(0, ux_flow_fee, NULL);
}

void ui_menu_change_validation_display(unsigned int value) {
  ux_flow_init(0, ux_flow_change, NULL);
}

/* ----------------------------- USER DEST/AMOUNT VALIDATION ----------------------------- */
void ui_menu_validation_action(unsigned int value);

UX_STEP_NOCB(
  ux_menu_validation_1_step,
  bn,
  {
    "Amout",
    G_electroneum_vstate.ux_amount
  });

UX_STEP_NOCB(
  ux_menu_validation_2_step,
  bnnn_paging,
  {
    "Destination",
    G_electroneum_vstate.ux_address
  });

UX_STEP_CB(
  ux_menu_validation_3_step,
  pb,
  ui_menu_validation_action(ACCEPT),
  {
    &C_icon_validate_14,
    "Accept"
  });

UX_STEP_CB(
  ux_menu_validation_4_step,
  pb,
  ui_menu_validation_action(REJECT),
  {
    &C_icon_crossmark,
    "Reject"
  });

UX_FLOW(ux_flow_validation,
  &ux_menu_validation_1_step,
  &ux_menu_validation_2_step,
  &ux_menu_validation_3_step,
  &ux_menu_validation_4_step
  );

void ui_menu_validation_display(unsigned int value) {
  ux_flow_init(0, ux_flow_validation, NULL);
}

void ui_menu_validation_action(unsigned int value) {
  unsigned short sw;
  if (value == ACCEPT) {
    sw = 0x9000;
  } else {
   sw = SW_SECURITY_STATUS_NOT_SATISFIED;
    electroneum_abort_tx();
  }
  electroneum_io_insert_u16(sw);
  electroneum_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
}

/* -------------------------------- EXPORT VIEW KEY UX --------------------------------- */
unsigned int ui_menu_export_viewkey_action(unsigned int value);

UX_STEP_NOCB(
  ux_menu_export_viewkey_1_step,
  nn,
  {
    "Export",
    "View Key"
  });

UX_STEP_CB(
  ux_menu_export_viewkey_2_step,
  pb,
  ui_menu_export_viewkey_action(ACCEPT),
  {
    &C_icon_validate_14,
    "Accept"
  });

UX_STEP_CB(
  ux_menu_export_viewkey_3_step,
  pb,
  ui_menu_export_viewkey_action(REJECT),
  {
    &C_icon_crossmark,
    "Reject"
  });

UX_FLOW(ux_flow_export_viewkey,
  &ux_menu_export_viewkey_1_step,
  &ux_menu_export_viewkey_2_step,
  &ux_menu_export_viewkey_3_step
  );

void ui_export_viewkey_display(unsigned int value) {
  ux_flow_init(0, ux_flow_export_viewkey,NULL);
}

unsigned int ui_menu_export_viewkey_action(unsigned int value) {
  unsigned int sw;
  unsigned char x[32];

  electroneum_io_discard(0);
  os_memset(x,0,32);
  sw = 0x9000;

  if (value == ACCEPT) {
    electroneum_io_insert(G_electroneum_vstate.a, 32);
    G_electroneum_vstate.export_view_key = EXPORT_VIEW_KEY;
  } else {
    electroneum_io_insert(x, 32);
    G_electroneum_vstate.export_view_key = 0;
  }
  electroneum_io_insert_u16(sw);
  electroneum_io_do(IO_RETURN_AFTER_TX);
  ui_menu_main_display(0);
  return 0;
}

/* -------------------------------- NETWORK UX --------------------------------- */

const char* const network_submenu_getter_values[] = {
  #ifdef ELECTRONEUM_ALPHA
  "Unvailable",
  #else
  "Main Network",
  #endif
  "Stage Network",
  "Test Network",
  "Abort"
};
const char* const network_submenu_getter_values_selected[] = {
  #ifdef ELECTRONEUM_ALPHA
  "Unvailable",
  #else
  "Main Network +",
  #endif
  "Stage Network +",
  "Test Network +",
  "Abort"
};


const char* network_submenu_getter(unsigned int idx) {
  if (idx >= ARRAYLEN(network_submenu_getter_values)) {
    return NULL;
  }
  int net;
  switch(idx) {
  case 0:
    #ifdef ELECTRONEUM_ALPHA
    net = -1;
    #else
    net = MAINNET;
    #endif
    break;
  case 1:
    net = STAGENET;
    break;
  case 2:
    net = TESTNET;
    break;
  default:
    net = -1;
    break;
  }
  if (N_electroneum_pstate->network_id == net) {
    return network_submenu_getter_values_selected[idx];
  } else {
    return network_submenu_getter_values[idx];
  }
}

void network_back(void) {
  ui_menu_main_display(0);
}

static void network_set_net(unsigned int network) {
  electroneum_install(network);
  electroneum_init();
}

void network_submenu_selector(unsigned int idx) {
  switch(idx) {
    case 0:
       #ifndef ELECTRONEUM_ALPHA
       network_set_net(MAINNET);
       #endif
       break;
    case 1:
       network_set_net(STAGENET);
       break;
    case 2:
       network_set_net(TESTNET);
       break;
    default:
      break;
  }
  ui_menu_main_display(0);
}


void ui_menu_network_display(unsigned int value) {
   ux_menulist_init(G_ux.stack_count-1, network_submenu_getter, network_submenu_selector);
}

void settings_change_network(void) {
  ui_menu_network_display(0);
}
/* -------------------------------- RESET UX --------------------------------- */
void ui_menu_reset_display(unsigned int value);
void ui_menu_reset_action(unsigned int value);

UX_STEP_NOCB(
  ux_menu_reset_1_step,
  nnn,
  {
    "",
    "Really Reset?",
    ""
  });

UX_STEP_CB(
  ux_menu_reset_2_step,
  pb,
  ui_menu_reset_action(REJECT),
  {
    &C_icon_crossmark,
    "No",
  });


UX_STEP_CB(
  ux_menu_reset_3_step,
  pb,
  ui_menu_reset_action(ACCEPT),
  {
    &C_icon_validate_14,
    "Yes",
  });

UX_FLOW(ux_flow_reset,
  &ux_menu_reset_1_step,
  &ux_menu_reset_2_step,
  &ux_menu_reset_3_step
  );

void ui_menu_reset_display(unsigned int value) {
   ux_flow_init(0, ux_flow_reset, 0);
}

void settings_reset(void) {
  ui_menu_reset_display(0);
}

void ui_menu_reset_action(unsigned int value) {
  if (value == ACCEPT) {
    unsigned char magic[4];
    magic[0] = 0; magic[1] = 0; magic[2] = 0; magic[3] = 0;
    electroneum_nvm_write((void*)N_electroneum_pstate->magic, magic, 4);
    electroneum_init();
  }
  ui_menu_main_display(0);
}
/* ------------------------------- SETTINGS UX ------------------------------- */

const char* const settings_submenu_getter_values[] = {
  "Change Network",
  "Show 25 words",
  "Reset",
  "Back",
};

const char* settings_submenu_getter(unsigned int idx) {
  if (idx < ARRAYLEN(settings_submenu_getter_values)) {
    return settings_submenu_getter_values[idx];
  }
  return NULL;
}

void settings_back(void) {
  ui_menu_main_display(0);
}

void settings_submenu_selector(unsigned int idx) {
  switch(idx) {
    case 0:
      settings_change_network();
      break;
    case 1:
      settings_show_25_words();
      break;
    case 2:
      settings_reset();
      break;
    default:
      settings_back();
  }
}



/* --------------------------------- INFO UX --------------------------------- */
#define STR(x)  #x
#define XSTR(x) STR(x)

UX_STEP_NOCB(
  ux_menu_info_1_step,
  bnnn,
  {
    "Electroneum",
    "(c) Ledger SAS",
    "Spec  " XSTR(SPEC_VERSION),
    "App  " XSTR(ELECTRONEUM_VERSION),
  });

UX_STEP_CB(
  ux_menu_info_2_step,
  pb,
  ui_menu_main_display(0),
  {
    &C_icon_back,
    "Back",
  });



UX_FLOW(ux_flow_info,
  &ux_menu_info_1_step,
  &ux_menu_info_2_step
  );


void ui_menu_info_display(unsigned int value) {
  ux_flow_init(0, ux_flow_info, NULL);
}

#undef STR
#undef XSTR





/* ---------------------------- PUBLIC ADDRESS UX ---------------------------- */
void ui_menu_pubaddr_action(unsigned int value);

#define ADDR_TYPE  G_electroneum_vstate.ux_wallet_public_address+108
#define ADDR_MAJOR G_electroneum_vstate.ux_wallet_public_address+124
#define ADDR_MINOR G_electroneum_vstate.ux_wallet_public_address+140
#define ADDR_IDSTR G_electroneum_vstate.ux_wallet_public_address+124
#define ADDR_ID    G_electroneum_vstate.ux_wallet_public_address+140

UX_STEP_NOCB(
  ux_menu_pubaddr_01_step,
  nn,
  {
    ADDR_TYPE,
    "Address",
  });


UX_STEP_NOCB(
  ux_menu_pubaddr_02_step,
  nn,
  {
    ADDR_MAJOR,
    ADDR_MINOR,
  });

UX_STEP_NOCB(
  ux_menu_pubaddr_1_step,
  bnnn_paging,
  {
    .title = "Address",
    .text = G_electroneum_vstate.ux_wallet_public_address
  });

UX_STEP_CB(
  ux_menu_pubaddr_2_step,
  pb,
  ui_menu_pubaddr_action(0),
  {
    &C_icon_back,
    "Ok"
  });

UX_FLOW(ux_flow_pubaddr,
  &ux_menu_pubaddr_01_step,
  &ux_menu_pubaddr_02_step,
  &ux_menu_pubaddr_1_step,
  &ux_menu_pubaddr_2_step
  );

void ui_menu_pubaddr_action(unsigned int value) {
  
  if (G_electroneum_vstate.disp_addr_mode) {
     electroneum_io_insert_u16(0x9000);
     electroneum_io_do(IO_RETURN_AFTER_TX);
  }  
  G_electroneum_vstate.disp_addr_mode = 0;
  ui_menu_main_display(0);
}

/**
 *
 */
void ui_menu_any_pubaddr_display(unsigned int value) {
  int l = 0;
  memset(G_electroneum_vstate.ux_wallet_public_address,0,sizeof(G_electroneum_vstate.ux_wallet_public_address));

  switch (G_electroneum_vstate.disp_addr_mode) {
  case 0:
  case DISP_MAIN:
    os_memmove(ADDR_TYPE, "Main", 4);
    os_memmove(ADDR_MAJOR, "Account: 0", 8);
    os_memmove(ADDR_MINOR, "SubAddr: 0", 8);
    l = 98;
    break;

  case DISP_SUB:
    os_memmove(ADDR_TYPE, "Sub", 3);
    snprintf(ADDR_MAJOR, 16, "Account: %d", G_electroneum_vstate.disp_addr_M);
    snprintf(ADDR_MINOR, 16, "SubAddr: %d", G_electroneum_vstate.disp_addr_m);
    l = 98;
    break;

  case DISP_INTEGRATED:
     os_memmove(ADDR_TYPE, "Integrated", 10);
     os_memmove(ADDR_IDSTR, "Payment ID", 10);
     os_memmove(ADDR_ID, G_electroneum_vstate.payment_id, 16);
     l = 106;
     break;
  }

  os_memmove(G_electroneum_vstate.ux_wallet_public_address+strlen(G_electroneum_vstate.ux_wallet_public_address),
             G_electroneum_vstate.ux_address,
             l);

  ux_layout_bnnn_paging_reset();
  ux_flow_init(0, ux_flow_pubaddr, NULL);
}

void ui_menu_pubaddr_display(unsigned int value) {
  memset(G_electroneum_vstate.ux_address,0,sizeof(G_electroneum_vstate.ux_address));
  electroneum_base58_public_key(G_electroneum_vstate.ux_address, G_electroneum_vstate.A,G_electroneum_vstate.B, 0, NULL);
  G_electroneum_vstate.disp_addr_mode = 0;
  G_electroneum_vstate.disp_addr_M = 0;
  G_electroneum_vstate.disp_addr_M = 0;
  ui_menu_any_pubaddr_display(value);
}

#undef ADDR_TYPE
#undef ADDR_MAJOR
#undef ADDR_MINOR
#undef ADDR_IDSTR
#undef ADDR_ID

/* --------------------------------- MAIN UX --------------------------------- */

UX_STEP_CB(
  ux_menu_main_1_step,
  pbb,
  ui_menu_pubaddr_display(0),
  {
    &C_icon_electroneum,
    "ETN",
    G_electroneum_vstate.ux_wallet_public_short_address
  });

UX_STEP_CB(
  ux_menu_main_2_step,
  pb,
  ux_menulist_init(G_ux.stack_count-1, settings_submenu_getter, settings_submenu_selector),
  {
    &C_icon_coggle,
    "Settings"
  });

UX_STEP_CB(
  ux_menu_main_3_step,
  pb,
  ui_menu_info_display(0),
  {
    &C_icon_certificate,
    "About"
  });

UX_STEP_CB(
  ux_menu_main_4_step,
  pb,
  os_sched_exit(0),
  {
    &C_icon_dashboard_x,
    "Quit app"
  });


UX_FLOW(ux_flow_main,
  &ux_menu_main_1_step,
  &ux_menu_main_2_step,
  &ux_menu_main_3_step,
  &ux_menu_main_4_step);

void ui_menu_main_display(unsigned int value) {
      // reserve a display stack slot if none yet
    if(G_ux.stack_count == 0) {
        ux_stack_push();
    }
     ux_flow_init(0, ux_flow_main, NULL);
}
/* --- INIT --- */

void ui_init(void) {
 ui_menu_main_display(0);
}

void io_seproxyhal_display(const bagl_element_t *element) {
  io_seproxyhal_display_default((bagl_element_t *)element);
}

#endif
