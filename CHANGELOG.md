# Changelog
All notable changes to this project will be documented in this file.

## [1.5.0] - 2021-10-29
### Added
- Added the ability to mark favourite radio stations and podcasts.
  A new list is created where you can find them and select one to
  listen to
- Added an asterisk above the play button that helps the users to
  add the selected podcast or radio station to the favourite list.
  It also works as an indicator if the selected item is already
  in the list or not.
- Added "Listen Later" list which will have the marked podcasts
  episodes which the user select from the Bookmark button,
  above the play button.
- Added all the necessary images into the archive, with
  Martin (Mason) Merz permission
- Added help hint texts at the play, bookmark and favourite
  buttons

### Changed
- Compiled with libcurl 7.79.1
- Removed the small boxes at the end of each line at the update
  changelog text
- Code simplified and cleaned in some places

### Fixed
- Fixed issues that could make MediaVault crash system at the
  image download section. This fixed also some images that were
  not downloaded. So now it will be able to show more images
  than before.
- When the radio station and podcast list was populated but a
  new search brought no results, the lists was not cleared and
  the choices were not usable. Now the list remains as it was,
  with the reults it had, but the choices are usable, as they
  were.


## [1.4.3] - 2021-10-21
### Changed
- Compiled with openssl to 1.1.1l

### Fixed
- Fixed the search of trending podcasts when the user didn't select
  any category and language
- Fixed the search of radio station when the user selected only the
  country and nothing else
- Fixed the script "start_player" execution when the MediaVault
  path contains spaces


## [1.4.2] - 2021-10-17
### Fixed
- The online radio stations api changed and broke the searching of
  online radio stations. This is a fix for that.


## [1.4.1] - 2021-10-10
### Added
- Added an error message when podcast response is not 200
- Added scripts/start_player.sample script. This can be used to set the
  preferred player. This needs to be renamed as start_player to be used.
  The reason the file is named like that is to avoid replacements in
  future updates.
  The file includes configuration for different supported players, but
  more can be added.
- Added Trending Podcasts search based on category and/or language
- Added radio station bitrate and codec information in results list
  and at the right sidebar.

### Changed
- Changed the about window to add more information.

### Fixed
- Fixed the error of not finding the texteditor.gadget on start


## [1.4.0] - 2021-10-05
### Added
- You can now search for podcasts based on their title, see info
  and their avatar and a list of the latest 100 episodes
- By selecting an episode and clicking the play button the audio
  is loaded to the player and starts playing

### Fixed
- Changed the way the avatar images are downloaded, which is now
  faster and solves a lot of errors with network access, which
  resulted connection error messages to appear.


## [1.3.0] - 2021-09-08
### Added
- Added right sidebar with station info and avatar image, wherever
  the image is available
- Added a new menu item to check for newer versions and start a
  selfupdate, if the user approves it. There is a way to see the
  latest changes before update.
- Added a play button at the right sidebar which is enabled as soon
  as the user selects a radio station. By clicking it the radio will
  be loaded at the default player. The double click on the radio
  list was disabled.

### Fixed
- Now the radio list is cleared if the search criteria change and
  no radio stations are found
- Now the list is not cleared if the "Discover More" button is
  clicked and no more stations were found
- Fixed a possible crash on window closing

### Changed
- Added a new message in case the "Discover More" button is clicked
  and no more stations were found
- Dropped oo.library network support for libcurl. That's why the
  executable size is increased so much
- Now oo.library and AmiSSL are not required
- Fixed the version string to include the application name
- On every request of radio stations, a list of 100 stations are
  returned and listed

## [1.2.1] - 2021-05-31
### Fixed
- Reduced results to 20 as a temporary workaround of a bug that
  occurs with the default discovery of radio stations. Found where
  the problem is and it will be fixed on a later release.
- Disabled the load of the window when AmiSSL 4.9 is missing
- Fixed the exit of MediaVault when a library is not available
- Fixed the issue with duplicate results when "discover more" was
  clicked
- Fixed a crash that had to do with a broken API response. Now an
  info requester appears.

## [1.2.0] - 2021-05-27
### Added
- Added Unicode conversion to system charset on radio station
  names - Thanks to Daniel "trixie" Jedlicka
- Now the application registers properly and can get requests from
  other apps, like Exchanger - Thanks to Daniel "trixie" Jedlicka
- Now the MediaVault can jump to different screens - Thanks to
  Daniel "trixie" Jedlicka
- Added better error manipulation when there were API response
  errors - Thanks to Daniel "trixie" Jedlicka
- Now the size and the position of the window can be saved by the
  Popup Gadget - Thanks to Daniel "trixie" Jedlicka

### Changed
- Added code to detach the listbrowser before change it's data.
  That's the way it should be done
- Restructured code to reduce the duplicated code in radiofuncs.c.
  Now, one method is used instead of three separated that had pretty
  much the same code. Better code, less problems.
- A lot of refactoring, removing global variables.
- Reduced a lot of duplicated code.
- Now the default radio station number increased to 50. The response
  is also much faster - Thanks to Tuomas Hokka and oo.library 1.13
  fixes
- Changed the way classes and gadgets are initiallized - Thanks to
  Daniel "trixie" Jedlicka

## [1.1.1] - 2021-05-15
### Changed
- Changed the required oo.library version to 1.12

### Fixed
- Fixed a bug on requesting radio stations without using the filters
  https://www.amigans.net/modules/xforum/viewtopic.php
    ?post_id=125112#forumpost125112
  https://www.amigans.net/modules/xforum/viewtopic.php
    ?post_id=125171#forumpost125171

## [1.1.0] - 2021-05-13
### Added
- Added a message requester for messages to the user
- The "Discover" button changes to "Discover more" after the first
  search, so to load more radio stations results. If the user change
  a filter, then it resets.
- Added more options at the Genre select box
- Added more options at the countries and languages select boxes
- Now the retrieved radio stations list doesn't include the marked
  as broken stations
- Added Trending list of radio stations
- Added Popular list of radio stations
- Changed columns' widths on listbrowsers to be weighted

## [1.0.0] - 2021-04-30
### Added
- Initial release
- Radio stations discovery is working
- Station url can be loaded in AmigaAmp3 only (for now)



The format is based on
[Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).
