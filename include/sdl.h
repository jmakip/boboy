#ifndef GB_WINDOW_SDL_H
#define GB_WINDOW_SDL_H

/**
 * Initialize gb window
 *
 */
void init_window();
void render_tilemap();
void poll_events();
/**
 * Initialize debug window
 *
 */
void debug_window_init();
/**
 * Update debug window
 *
 */
void debug_window_update();

//debug print of OAM memory
void dump_OAM();
#endif
