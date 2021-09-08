# Changelog
All notable changes to this project will be documented in this file.

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
