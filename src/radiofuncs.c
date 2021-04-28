/*

************************************************************
**
** Created by: codebench 0.55 (19.10.2017)
**
** Project: MediaVault
**
** File: radiofuncs.c
**
** Date: 24-04-2021 10:01:11
**
************************************************************

*/

#include "globals.h"
#include "libshandler.h"
#include "httpfuncs.h"

static CONST_STRPTR radioAPIUrl = "https://de1.api.radio-browser.info/json";
//static CONST_STRPTR s_data_b = "[{\"changeuuid\":\"723a4a07-aec5-4a14-9262-b198a6be579c\",\"stationuuid\":\"5a7457fb-8736-4926-afc5-3a1647ec3eed\",\"name\":\"__80s Live__\",\"url\":\"https://16963.live.streamtheworld.com/SAM03AAC226_SC?__80sradio__\",\"url_resolved\":\"https://16963.live.streamtheworld.com/SAM03AAC226_SC?__80sradio__\",\"homepage\":\"https://www.80sradio.co.uk/\",\"favicon\":\"https://lh6.googleusercontent.com/clbJCcZIC54IPc7PDUSM-SGjZkLcHMjcidQaPFICPQbQFjVKitGQ-wSgv-lopKLp3m0d2H-0TnbQuH9poRtmgPmI069KaLOjGzKxyN22EZeK-99mDepj8LOuwlMYEiCeUQ=w1280\",\"tags\":\"1980s,live uk radio,80s live radio,live 80s radio,the best decade 80s radio around.,pop,new,wave,synth,rock,pop,live,eighties,public radio,1980s,80er,80s,80s\",\"country\":\"The United Kingdom Of Great Britain And Northern Ireland\",\"countrycode\":\"GB\",\"state\":\"\",\"language\":\"english\",\"languagecodes\":\"\",\"votes\":11,\"lastchangetime\":\"2021-03-06 04:06:45\",\"lastchangetime_iso8601\":\"2021-03-06T04:06:45Z\",\"codec\":\"MP3\",\"bitrate\":128,\"hls\":0,\"lastcheckok\":1,\"lastchecktime\":\"2021-04-22 19:34:00\",\"lastchecktime_iso8601\":\"2021-04-22T19:34:00Z\",\"lastcheckoktime\":\"2021-04-22 19:34:00\",\"lastcheckoktime_iso8601\":\"2021-04-22T19:34:00Z\",\"lastlocalchecktime\":\"2021-04-22 00:26:19\",\"lastlocalchecktime_iso8601\":\"2021-04-22T00:26:19Z\",\"clicktimestamp\":\"2021-04-22 21:20:36\",\"clicktimestamp_iso8601\":\"2021-04-22T21:20:36Z\",\"clickcount\":27,\"clicktrend\":1,\"ssl_error\":0,\"geo_lat\":null,\"geo_long\":null},{\"changeuuid\":\"e9b612d5-23b8-11ea-aa0c-52543be04c81\",\"stationuuid\":\"e9b612ab-23b8-11ea-aa0c-52543be04c81\",\"name\":\"# 1980s Radio Zoom - 64k Stereo / 128k Mono [Harlow]\",\"url\":\"https://samcloud.spacial.com/api/listen?sid=111415&rid=195121&f=mp3,any&br=128000,any&m=m3u\",\"url_resolved\":\"http://16963.live.streamtheworld.com/SAM03AAC226_SC\",\"homepage\":\"http://www.80sradio.co.uk/\",\"favicon\":\"https://lh5.googleusercontent.com/UTtctfY10aHUte3KXSwRrfhhR6sZwmfHsNnbbWPbMXNQ2IuYksdpKy3Hh1dh4TED3tlYn55Ft7U2CNvR2utOCJxkvoun1If1Nl0uNoHRQQWoJTa0CJu1=w1280\",\"tags\":\"essex,harlow,mp3,128kbit,64kbps,vintage,eighties,80s pop,1980s pop,24hour,freeform,no adverts,commercial free,non stop,retro,hit music,oldies,rock,pop,80s\",\"country\":\"The United Kingdom Of Great Britain And Northern Ireland\",\"countrycode\":\"GB\",\"state\":\"Harlow\",\"language\":\"english\",\"languagecodes\":\"\",\"votes\":1969,\"lastchangetime\":\"2021-02-07 13:11:48\",\"lastchangetime_iso8601\":\"2021-02-07T13:11:48Z\",\"codec\":\"MP3\",\"bitrate\":128,\"hls\":0,\"lastcheckok\":1,\"lastchecktime\":\"2021-04-22 08:28:31\",\"lastchecktime_iso8601\":\"2021-04-22T08:28:31Z\",\"lastcheckoktime\":\"2021-04-22 08:28:31\",\"lastcheckoktime_iso8601\":\"2021-04-22T08:28:31Z\",\"lastlocalchecktime\":\"2021-04-22 01:05:18\",\"lastlocalchecktime_iso8601\":\"2021-04-22T01:05:18Z\",\"clicktimestamp\":\"2021-04-22 22:19:35\",\"clicktimestamp_iso8601\":\"2021-04-22T22:19:35Z\",\"clickcount\":388,\"clicktrend\":0,\"ssl_error\":0,\"geo_lat\":null,\"geo_long\":null},{\"changeuuid\":\"edb72b13-23b7-11ea-aa0c-52543be04c81\",\"stationuuid\":\"edb72af6-23b7-11ea-aa0c-52543be04c81\",\"name\":\"# Radio 80s Zoom - 128k .mp3 Stereo [Southampton]\",\"url\":\"https://samcloud.spacial.com/api/listen?sid=111415&rid=194939&f=mp3,any&br=128000,any&m=pls\",\"url_resolved\":\"http://13683.live.streamtheworld.com/SAM03AAC226_SC\",\"homepage\":\"http://radioroomz.com/\",\"favicon\":\"https://lh5.googleusercontent.com/UTtctfY10aHUte3KXSwRrfhhR6sZwmfHsNnbbWPbMXNQ2IuYksdpKy3Hh1dh4TED3tlYn55Ft7U2CNvR2utOCJxkvoun1If1Nl0uNoHRQQWoJTa0CJu1=w1280\",\"tags\":\"nostalgie,nostalgia,commercial free,no adverts,non stop,128kbit,mp3,paisa estereo,classic hits,hits,classic rock,pop,rock,decades,eighties,1980s,80s\",\"country\":\"The United Kingdom Of Great Britain And Northern Ireland\",\"countrycode\":\"GB\",\"state\":\"Southampton\",\"language\":\"italian,german,spanish,french,english\",\"languagecodes\":\"\",\"votes\":382,\"lastchangetime\":\"2021-02-07 13:11:48\",\"lastchangetime_iso8601\":\"2021-02-07T13:11:48Z\",\"codec\":\"MP3\",\"bitrate\":128,\"hls\":0,\"lastcheckok\":1,\"lastchecktime\":\"2021-04-22 09:35:56\",\"lastchecktime_iso8601\":\"2021-04-22T09:35:56Z\",\"lastcheckoktime\":\"2021-04-22 09:35:56\",\"lastcheckoktime_iso8601\":\"2021-04-22T09:35:56Z\",\"lastlocalchecktime\":\"2021-04-22 03:30:56\",\"lastlocalchecktime_iso8601\":\"2021-04-22T03:30:56Z\",\"clicktimestamp\":\"2021-04-22 22:12:59\",\"clicktimestamp_iso8601\":\"2021-04-22T22:12:59Z\",\"clickcount\":26,\"clicktrend\":-6,\"ssl_error\":0,\"geo_lat\":null,\"geo_long\":null},{\"changeuuid\":\"a726f025-20c8-11ea-a955-52543be04c81\",\"stationuuid\":\"a726f016-20c8-11ea-a955-52543be04c81\",\"name\":\"#80s Radio - Icecast 2\",\"url\":\"https://samcloud.spacial.com/api/listen?sid=111415&rid=194935&f=mp3,any&br=320000,any&m=pls\",\"url_resolved\":\"http://13683.live.streamtheworld.com/SAM03AAC226_SC\",\"homepage\":\"http://www.80sradio.co.uk/\",\"favicon\":\"https://mytuner.global.ssl.fastly.net/media/tvos_radios/cjqfbpl6lyyn.png\",\"tags\":\"commercial-free,commercial-free,non-commercial,no ads,no advertising,non-stop music,non-stop music,hit music,80s hits,best of 80s,decades,eighties,rock pop,goldies,golden oldies,oldies,new wave,rock,pop,classic rock,classic pop,vintage,retro,1980s,80s\",\"country\":\"The United Kingdom Of Great Britain And Northern Ireland\",\"countrycode\":\"GB\",\"state\":\"Southampton\",\"language\":\"spanish,italian,german,french,english\",\"languagecodes\":\"\",\"votes\":678,\"lastchangetime\":\"2021-02-07 13:11:48\",\"lastchangetime_iso8601\":\"2021-02-07T13:11:48Z\",\"codec\":\"MP3\",\"bitrate\":128,\"hls\":0,\"lastcheckok\":1,\"lastchecktime\":\"2021-04-22 17:59:42\",\"lastchecktime_iso8601\":\"2021-04-22T17:59:42Z\",\"lastcheckoktime\":\"2021-04-22 17:59:42\",\"lastcheckoktime_iso8601\":\"2021-04-22T17:59:42Z\",\"lastlocalchecktime\":\"2021-04-22 17:59:42\",\"lastlocalchecktime_iso8601\":\"2021-04-22T17:59:42Z\",\"clicktimestamp\":\"2021-04-22 22:20:58\",\"clicktimestamp_iso8601\":\"2021-04-22T22:20:58Z\",\"clickcount\":110,\"clicktrend\":0,\"ssl_error\":0,\"geo_lat\":null,\"geo_long\":null},{\"changeuuid\":\"427712a8-23b6-11ea-aa0c-52543be04c81\",\"stationuuid\":\"4277129d-23b6-11ea-aa0c-52543be04c81\",\"name\":\"#80s Radio Zoom AAC Stereo 64k\",\"url\":\"https://samcloud.spacial.com/api/listen?sid=111415&rid=191984&f=heaacv2,any&br=64000,any&m=pls\",\"url_resolved\":\"http://13683.live.streamtheworld.com/SAM03AAC226_SC\",\"homepage\":\"http://internetradio.fun/\",\"favicon\":\"https://lh4.googleusercontent.com/1c9Qw7ObpqFskf67Edb8a6Db6UYXcvHlq_fEO28A7m-7GA3M3uBm0zpFzy2g8xZ1SBI9bc3rPaJLVPfCQ5NeryJc8aBRFGdm93HZ8FVhjq8McU5qF_dT=w1280\",\"tags\":\"24hour,non stop,freeform,no adverts,no commercials,jesus alive radio,1980s,eighties,easy listening,classic rock,rock,decades,pop,80s\",\"country\":\"The United Kingdom Of Great Britain And Northern Ireland\",\"countrycode\":\"GB\",\"state\":\"Bristol\",\"language\":\"english\",\"languagecodes\":\"\",\"votes\":429,\"lastchangetime\":\"2021-02-07 13:11:48\",\"lastchangetime_iso8601\":\"2021-02-07T13:11:48Z\",\"codec\":\"MP3\",\"bitrate\":128,\"hls\":0,\"lastcheckok\":1,\"lastchecktime\":\"2021-04-22 05:16:11\",\"lastchecktime_iso8601\":\"2021-04-22T05:16:11Z\",\"lastcheckoktime\":\"2021-04-22 05:16:11\",\"lastcheckoktime_iso8601\":\"2021-04-22T05:16:11Z\",\"lastlocalchecktime\":\"2021-04-22 03:27:28\",\"lastlocalchecktime_iso8601\":\"2021-04-22T03:27:28Z\",\"clicktimestamp\":\"2021-04-22 22:31:10\",\"clicktimestamp_iso8601\":\"2021-04-22T22:31:10Z\",\"clickcount\":24,\"clicktrend\":-6,\"ssl_error\":0,\"geo_lat\":null,\"geo_long\":null}]";

extern struct List radioList;
extern int32 radioListItemsCnt;

STRPTR getRadioStations(char *selName, char *selGenre, char *selLanguage, char *selCountry)
{
  char url[255];
	
	IUtility->Strlcpy(url, radioAPIUrl, sizeof(url));
	IUtility->Strlcat(url, "/stations/search?limit=20", sizeof(url));
	if (IUtility->Stricmp(selName, ""))
	{
	  STRPTR encSelName = urlEncode(selName);
	  IUtility->Strlcat(url, "&name=", sizeof(url));
	  IUtility->Strlcat(url, encSelName, sizeof(url));
	  IExec->FreeVec(encSelName);
	}
	if (IUtility->Stricmp(selGenre, ""))
	{
	  STRPTR encSelGenre = urlEncode(selGenre);
	  IUtility->Strlcat(url, "&tag=", sizeof(url));
	  IUtility->Strlcat(url, encSelGenre, sizeof(url));
	  IExec->FreeVec(encSelGenre);
	}
	if (IUtility->Stricmp(selLanguage, ""))
	{
	  STRPTR encSelLanguage = urlEncode(selLanguage);
	  IUtility->Strlcat(url, "&language=", sizeof(url));
	  IUtility->Strlcat(url, encSelLanguage, sizeof(url));
	  IExec->FreeVec(encSelLanguage);
	}
	if (IUtility->Stricmp(selCountry, ""))
	{
	  STRPTR encSelCountry = urlEncode(selCountry);
	  IUtility->Strlcat(url, "&country=", sizeof(url));
	  IUtility->Strlcat(url, encSelCountry, sizeof(url));
	  IExec->FreeVec(encSelCountry);
	}
	
	//IDOS->Printf("%s\n", url);

	return getResponseBody(url, NET_PORT_HTTPS);
}

void getRadioList(STRPTR jsonData)
{
  struct Node *stationNode; 
	json_t *jsonRoot;
	json_error_t jsonError;    	
  size_t i;
 			
  jsonRoot = IJansson->json_loads(jsonData, 0, &jsonError);
   		
  if(!jsonRoot)
 	{
 		IDOS->Printf("json error: on line %d: %s\n", jsonError.line, jsonError.text);
  	CleanExit("JSON Error");
 	}
 			
 	if (!json_is_array(jsonRoot))
  {
		IJansson->json_decref(jsonRoot);
		CleanExit("JSON error: jsonRoot is not an array");
	}
  json_t *data, *stationuuid, *name, *country, *tags, *url_resolved, *votes;
  ULONG votesNum = 0;
  
  IExec->NewList(&radioList);
  radioListItemsCnt = 0;
   		
	for(i = 0; i < IJansson->json_array_size(jsonRoot); i++)
	{
		data = IJansson->json_array_get(jsonRoot, i);
		if(!json_is_object(data))
		{
			IDOS->Printf("error: commit data %d is not an object\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		
		stationuuid = IJansson->json_object_get(data, "stationuuid");
		if(!json_is_string(stationuuid))
		{
			IDOS->Printf("error: commit %d: stationuuid is not a string\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("stationuuid: %s\n", IJansson->json_string_value(stationuuid));
		
		name = IJansson->json_object_get(data, "name");
		if(!json_is_string(name))
		{
			IDOS->Printf("error: commit %d: name is not a string\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("Station name: %s\n", IJansson->json_string_value(name));
		
		country = IJansson->json_object_get(data, "country");
		if(!json_is_string(country))
		{
			IDOS->Printf("error: commit %d: country is not a string\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("Station country: %s\n", IJansson->json_string_value(country));
		
		tags = IJansson->json_object_get(data, "tags");
		if(!json_is_string(tags))
		{
			IDOS->Printf("error: commit %d: tags is not a string\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("Station tags: %s\n", IJansson->json_string_value(tags));
		
		url_resolved = IJansson->json_object_get(data, "url_resolved");
		if(!json_is_string(url_resolved))
		{
			IDOS->Printf("error: commit %d: url_resolved is not a string\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("Station url_resolved: %s\n", IJansson->json_string_value(url_resolved));

		votes = IJansson->json_object_get(data, "votes");
		if(!json_is_integer(votes))
		{
			IDOS->Printf("error: commit %d: votes is not an integer\n", (int)(i + 1));
			IJansson->json_decref(jsonRoot);
			CleanExit("JSON Error");
		}
		//IDOS->Printf("votes: %ld\n", (ULONG)IJansson->json_integer_value(votes));
		votesNum = (ULONG)IJansson->json_integer_value(votes);
		
		stationNode = IListBrowser->AllocListBrowserNode( 4,
				LBNA_Column, 0,
					LBNCA_CopyText, TRUE,
					LBNCA_Text, IJansson->json_string_value(name),
				LBNA_Column, 1,
					LBNCA_CopyText, TRUE,
					LBNCA_Text, IJansson->json_string_value(country),
				LBNA_Column, 2,
					LBNCA_CopyInteger, TRUE,
					LBNCA_Integer, &votesNum,
				LBNA_Column, 3,
					LBNCA_CopyText, TRUE,
					LBNCA_Text, IJansson->json_string_value(url_resolved),
				TAG_DONE);

		if(stationNode)
		{
		  radioListItemsCnt++;
			IExec->AddTail(&radioList, stationNode);
		}  
	}
      		
	IJansson->json_decref(jsonRoot);			
}
