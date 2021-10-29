![GitHub release (latest by date)](https://img.shields.io/github/v/release/walkero-gr/mediavault?label=MediaVault)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/634c72a5f74142228ed82c47848ba17d)](https://app.codacy.com/gh/walkero-gr/mediavault?utm_source=github.com&utm_medium=referral&utm_content=walkero-gr/mediavault&utm_campaign=Badge_Grade_Settings)
[![ko-fi](https://img.shields.io/badge/-Buy%20me%20a%20coffee-blue?style=flat&logo=ko-fi&logoColor=red-light)](https://ko-fi.com/walkero)

# mediavault

MediaVault is an open source desktop application for AmigaOS 4, which can be
used to discover online audio and video content.

With MediaVault you can discover radio stations and podcasts and listen to
them using your audio player of choice. There is different ways to searh and
filter results, based on language, category/genre.

The user interface is easy to understand and use. At the left side there is
a list about the different type of content. At the middle section there are
filters and selections at the top, that help you find the content you are
looking for, and the result of the search is shown at the list below. By
selecting an item (radio station/podcast) from the lists, information show
up at the right sidebar, where there is a play button for the radio stations
or a second list of episodes for the podcasts. When the play button is
enabled, it can be used for the player to start playing the selected audio.

![preview](img/mediavault_1.3.0.gif)

Set your favorite player
--------------------------
MediaVault uses a script under the folder scripts, named start_player. To
enable that feature, you have to rename start_player.sample file to
start_player, otherwise it is going to be ignored by MediaVault. This is
necessary process, so that it will not be replace by future releases. You
have to do it only once.

By default, AmigaAmp3 is set to be used, but you can edit the file and set
your audio player of choice. In that file you will find configuration for
other known players, like Emotion and ffplay, that were tested and it is
known to work fine. To enable one of them you have to remove the
semicolons (;) at the start of their lines. Also, you have to add
semicolons at the start of their lines for the rest of the enabled players,
so to prevent multiple of them starting at the same time. Or you can remove
their respective lines.

You can change the start_player script files as you want. You can add your
own players, if you like.

If you need help, please, do not hesitate to contact me.

Podcast do not work?
--------------------------
If you search for a podcast but nothing comes up, then you will see a
requester with information about possible reasons that might prevent it
to work.

The online service MediaVault uses to get information about podcasts,
requires a timestamp for every request to be sent. This means that your
computer has to have the right date and time set. Please, use Workbench
Time preferences, or any other program you prefer, to sync your system
date and time with an online NTP server.

If this still doesn't work for you, please open an issue at MediaVault's
GitHub page and write down as much information as you can for the problem
you experience. You can find the exact url below.

Vision
--------------------------
My plan for MediaVault is to create an application which users will use to:
- discover online radio stations,
- create a list of favorite radio stations
- discover online podcasts,
- follow podcasts and get information for new episodes
- access content (music, video etc.) from local DLNA media servers, like
  Plex, Jellyfin etc.
- have a modern look and feel, with images and photos, where it is possible
- create a clean, easy to read code, with good quality and the best practices
  for others to get inspired

Requirements
--------------------------
* jansson.library v2.12.1 by Simon Tyrrell
* AISS by Martin (Mason) Merz
* An audio/video player that supports URL streaming and HTTPS, like ffplay,
  Emotion and AmigaAmp3

I would like to thank
--------------------------
- Tuomas Hokka for developing oo.library and listen to my requirements, ideas.
- Simon Tyrrell for developing jansson.library and his help on debugging
  issues I had during development, listen to my ideas.
- Thomas Wenzel for developing the best audio player for AmigaOS, the AmigaAMP
- Simon Archer who develops CodeBench, which helped me a lot during the
  development of this application                         
- Daniel "trixie" Jedlicka who worked on a lot of features, fixes and 
  refactoring of MediaVault, providing proposals, ideas and information
  to my questions
- Michael Trebilcock for his ports of libcurl, librtmp, libz and
  libopenssl
- Martin (Mason) Merz for his AISS icons
- IconDesigner for the proper glow icons that are included in the release
- Everyone at os4coding.net, who helped with proposals and solutions on
  my questions.
- AmigaOS 4 developers who develop the best OS out there.

Without all the above MediaVault would not be possible.

Code and bug reporting
--------------------------
The project is open source and you can find the code at:
https://github.com/walkero-gr/mediavault

If you have any requests or you would like to report any problems you found,
you can do that at:
https://github.com/walkero-gr/mediavault/issues

Help me persuade my wife that what I am doing matters
-----------------------------------------------------------
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/W7W465887)

Changelog
--------------------------
v1.5.0 - 2021-10-29
* Added the ability to mark favourite radio stations and podcasts.
  A new list is created where you can find them and select one to
  listen to
* Added an asterisk above the play button that helps the users to
  add the selected podcast or radio station to the favourite list.
  It also works as an indicator if the selected item is already
  in the list or not.
* Added all the necessary images into the archive, with
  Martin (Mason) Merz permission
* Added help hint texts at the play, bookmark and favourite
  buttons
* Compiled with libcurl 7.79.1
* Removed the small boxes at the end of each line at the update
  changelog text
* Code simplified and cleaned in some places
* Fixed issues that could make MediaVault crash system at the
  image download section. This fixed also some images that were
  not downloaded. So now it will be able to show more images
  than before.
* When the radio station and podcast list was populated but a
  new search brought no results, the lists was not cleared and
  the choices were not usable. Now the list remains as it was,
  with the reults it had, but the choices are usable, as they
  were.

v1.4.3 - 2021-10-21
* Compiled with openssl to 1.1.1l
* Fixed the search of trending podcasts when the user didn't select
  any category and language
* Fixed the search of radio station when the user selected only the
  country and nothing else
* Fixed the script "start_player" execution when the MediaVault
  path contains spaces

v1.4.2 - 2021-10-17
* The online radio stations api changed and broke the searching of
  online radio stations. This is a fix for that.

v1.4.1 - 2021-10-10
* Added an error message when podcast response is not a succesful one
* Added scripts/start_player.sample script. This can be used to set the
  preferred player. This needs to be renamed as start_player to be used.
  The reason the file is named like that is to avoid replacements in
  future updates.
  The file includes configuration for different supported players, but
  more can be added.
* Added Trending Podcasts search based on category and/or language
* Added radio station bitrate and codec information in results list
  and at the right sidebar.
* Changed the about window to add more information.
* Fixed the error of not finding the texteditor.gadget on start

v1.4.0 - 2021-10-05
* You can now search for podcasts based on their title, see info and their
  avatar and a list of the latest 100 episodes
* By selecting an episode and clicking the play button the audio is loaded to
  the player and starts playing
* Changed the way the avatar images are downloaded, which is now faster and
  solves a lot of errors with network access, which resulted connection error
  messages to appear.

v1.3.0 - 2021-09-08
* Added right sidebar with station info and avatar image, wherever the image is available
* Added a new menu item to check for newer versions and start a selfupdate, if the user approves it. There is a way to see the latest changes before update.
* Now the radio list is cleared if the search criteria change and no radio stations are found
* Now the list is not cleared if the "Discover More" button is clicked and no more stations were found
* Fixed a possible crash on window closing
* Added a new message in case the "Discover More" button is clicked and no more stations were found
* Dropped oo.library network support for libcurl, which more mature and has a lot more options and features. Unfortunately that increased the executable size a lot.
* Now oo.library and AmiSSL are not required
* Fixed the version string to include the application name
* Added a play button at the right sidebar which is enabled as soon as the user selects a radio station. By clicking it the radio will be loaded at the default player. The double click on the radio list was disabled.
* On every request of radio stations, a list of 100 stations are returned and listed

