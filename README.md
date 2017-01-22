README
======

	1. Introduction
	2. Features
	3. Prerequisites
	4. Dependencies
	5. Building
	6. Configuraton
	7. Bugs
	8. Authors


INTRODUCTION
============

   Meawm was forked from Waimea, whose goal was to create the most
   efficient desktop working environment available. To achieve this,
   waimea was made fast and very customizable virtual multiple
   desktop window manager. 

   Meawm has a very advanced style engine with features like blackbox
   style support, pixmap style support and transparent textures.
   Text can be rendered double buffered using both X core fonts and
   Xft fonts. Meawm also includes a fast menu system with dynamic menus
   support. The built in action configuration system makes meawm the
   most configurable window manager available. It allows the user to
   set up meawm to behave as any other window manager or in new ways
   never before possible.


Features
========

	- Virtual desktop sizes
	- Multiple desktops
	- Blackbox image rendering engine
	- Pixmap styles
	- Translucent textures using Xrender extension
	- Action configuration system
	- Advanced menu system (with dynamic menus support)
   	- Standard X core fonts
	- Xft fonts (anti-aliased fonts)
	- Double buffered text
	- Dockapp handler system [compilation defined]
	- Task switcher [compilation defined]
	- Configurable titlebar buttons
	- Merged windows [compilation defined]


DEPENDENCIES
============

	* Xorg or xfree86
	* imlib2 1.0.6+
	* Freetype 1.12+
	* Cairo 1.14+


BUILDING
========

	Install meawm by running the following commands:

	----- cut here -----
	./configure --prefix=/usr --enable-shape --enable-xinerama \
		--enable-render --enable-randr --enable-xft \
		--enable-pixmap &&
	make &&
	make install
	----- cut here -----

	Command explanations

	--enable-shape : This command activates meawm's ability 
	to support non-rectangular windows.

	--enable-xinerama :  This  command  activates  meawm's 
	ability to support Xinerama screens.

	--enable-render :   This  command  activates   meawm's 
	ability to render transparent textures.

	--enable-randr : This command activates meawm's ability
	to handle dynamic screen size changes.

	--enable-xft : This command activates meawm's ability to
	render anti-aliased text.

	--enable-pixmap : This command activates meawm's ability
	to use pixmap styles.


CONFIGURATION
=============

	Config files
	------------

	~/.meawmrc, ~/.meawm/actions/action, 
	~/.meawm/styles/Default.style, ~/.meawm/menu

	Configuration Information
	-------------------------

	Be sure to backup your current .xinitrc before proceeding.

   ----- cut here -----
	cat >> ~/.xinitrc << "EOF"
	exec meawm
	EOF
   ----- cut here -----

	----- cut here -----
	cat >> ~/.meawmrc << "EOF"
	screenMask:          0 1 2 3
	doubleClickInterval: 300

	screen0.styleFile:          ~/.meawm/styles/Default.style
	screen0.menuFile:           ~/.meawm/menu
	screen0.actionFile:         ~/.meawm/actions/action
   screen0.numberOfDesktops:   4
   screen0.desktopNames:       Desktop 1,Desktop 2,Desktop 3,Desktop 4
   screen0.doubleBufferedText: True
	screen0.colorsPerChannel:   8
	screen0.cacheMax:           200
	screen0.imageDither:        True
	screen0.virtualSize:        3x3
	screen0.menuStacking:       Normal
	screen0.transientAbove:     True
   screen0.focusRevertTo:      Window
	screen0.dock0.geometry:     -0+0
	screen0.dock0.order:        n/.*/
   screen0.dock0.desktopMask:  All
	screen0.dock0.centered:     False
	screen0.dock0.direction:    Vertical
	screen0.dock0.gridSpace:    2
	screen0.dock0.stacking:     AlwaysOnTop
	screen0.dock0.inworkspace:  False
	EOF
	----- cut here -----

	----- cut here -----
	mkdir ~/.meawm &&
	mkdir ~/.meawm/styles &&
	mkdir ~/.meawm/actions &&
	cp /usr/share/meawm/styles/Default.style ~/.meawm/styles/Default.style &&
	cp /usr/share/meawm/actions/action ~/.meawm/actions/action &&
	cp /usr/share/meawm/menu ~/.meawm/menu
	----- cut here -----

	Configuration  options are set by  editing `~/.meawmrc'. 
	Available options  and syntax is  explained in the meawm man page.

	Key and mouse bindings are set by editing `~/.meawm/actions/action'.
	The syntax is explained in the  meawm man page.

	Menu Items are added by editing `~/.meawm/menu'. 
	The syntax is explained in the meawm man page.


BUGS
====
   Bug reports, patches and suggestions are much appreciated, 
   See the GitHub account https://www.github.com/dev-breeze-com/meawm


AUTHOR
======
   Pierre Innocent ( dev@breezeos.com ) 
   The Breeze::OS website: http://www.breezeos.com  


ORIGINAL AUTHOR 
===============
   David Reveman ( david@waimea.org )  
   The waimea website: http://www.waimea.org  

SUPPORT
=======
   Support this project by providing a better icon, than the one
   that is contained in the file data/meawm-48x48.jpg

   Also can also donate to support this project at 
<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_s-xclick">
<input type="hidden" name="hosted_button_id" value="TZU8BE63SVVL4">
<input type="image"
src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" border="0"
name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0"
src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1"
height="1">
</form>

