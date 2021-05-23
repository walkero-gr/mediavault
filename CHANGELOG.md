# Changelog
All notable changes to this project will be documented in this file.

## [1.2.0] - future
### Added
- Added Unicode conversion to system charset on radio station names

### Changed
- Added code to detach the listbrowser before change it's data. That's the way it should be done
- Restructured code to reduce the duplicated code in radiofuncs.c. Now, one method is used instead of three separated that had pretty much the same code. Better code, less problems.
- Refactoring continues. Added listCount() and removed some global variables.
- Reduced a lot of duplicated code.

## [1.1.1] - 2021-05-15
### Changed
- Changed the required oo.library version to 1.12

### Fixed
- Fixed a bug on requesting radio stations without using the filters
  https://www.amigans.net/modules/xforum/viewtopic.php?post_id=125112#forumpost125112
  https://www.amigans.net/modules/xforum/viewtopic.php?post_id=125171#forumpost125171

## [1.1.0] - 2021-05-13
### Added
- Added a message requester for messages to the user
- The "Discover" button changes to "Discover more" after the first search, so to load more radio stations results. If the user change a filter, then it resets.
- Added more options at the Genre select box
- Added more options at the countries and languages select boxes
- Now the retrieved radio stations list doesn't include the marked as broken stations
- Added Trending list of radio stations
- Added Popular list of radio stations
- Changed columns' widths on listbrowsers to be weighted

## [1.0.0] - 2021-04-30
### Added
- Initial release
- Radio stations discovery is working
- Station url can be loaded in AmigaAmp3 only (for now)



The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
