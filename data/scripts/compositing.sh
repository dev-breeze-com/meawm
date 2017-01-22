#!/bin/sh

exec compton \
	-i 0.50 -e 0.80 -m 1.0 -f -b -c -G \
	--xrender-sync --force-win-blend --xrender-sync-fence \
	--detect-transient --sw-opti \
	--shadow-exclude "name = 'DESKTOP_CLOCK'" \
	--shadow-exclude "window_type = 'popup'" \
	--focus-exclude "name = 'DESKTOP_CLOCK'" \
	--shadow-exclude "class_g = 'Conky'" \
	--shadow-exclude "name = 'Meawm Menu'" \
	--shadow-exclude "name = 'REMINDER'" \
	--focus-exclude "name = 'REMINDER'" \
	--shadow-exclude "name = 'meawm'" \
	--focus-exclude "name = 'meawm'"

