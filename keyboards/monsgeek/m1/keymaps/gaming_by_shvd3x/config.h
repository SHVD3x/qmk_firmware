/*
Copyright 2023 SHVD3x

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

///#define FORCE_NKRO

//#define RGB_MATRIX_TIMEOUT 1200000

//#define DEBOUNCE 2

#define LAYER_STATE_8BIT // Limits max number of layers to 8. Reduces keymap size.

#define NO_ACTION_ONESHOT //If you need this feature just remove this string

//#define DEBOUNCE 2 		//Uncomment to set your preferred debounce time. Default is 5. Value is set in ms.
//#undef RGB_MATRIX_LED_FLUSH_LIMIT 
//#define RGB_MATRIX_LED_FLUSH_LIMIT 8
//#define RGB_MATRIX_LED_PROCESS_LIMIT (RGB_MATRIX_LED_COUNT + 4) / 5 // - тут нужно во первых узнать какое значение будет вообще правильно ставить, а во вторых ты пидор

/*
#undef ENABLE_RGB_MATRIX_ALPHAS_MODS
#undef ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#undef ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_BAND_SAT
#undef ENABLE_RGB_MATRIX_BAND_VAL
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#undef ENABLE_RGB_MATRIX_PIXEL_FRACTAL
#undef ENABLE_RGB_MATRIX_PIXEL_FLOW
#undef ENABLE_RGB_MATRIX_DIGITAL_RAIN
#undef ENABLE_RGB_MATRIX_TYPING_HEATMAP
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#undef ENABLE_RGB_MATRIX_SPLASH
#undef ENABLE_RGB_MATRIX_SOLID_SPLASH
*/

#ifdef CONSOLE_ENABLE
	#define DEBUG_MATRIX_SCAN_RATE
	#define KEYBOARD_SHARED_EP
#endif
