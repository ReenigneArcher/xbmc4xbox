__all__ = [
    # public names

    "PluginMgr"
    ]

#Modules general
import os
import sys
import urllib

from traceback import print_exc

# timeout in seconds
timeout = 3


#modules XBMC
#import xbmc
import xbmcgui
import xbmcplugin

# Custom modules
try:
    from globalvars import PARAM_NAME, PARAM_ACTION, VALUE_DISPLAY_INFO
except:
    print_exc()




#BASE_SETTINGS_PATH = os.path.join( sys.modules[ "__main__" ].SPECIAL_SCRIPT_DATA, "settings.txt" )
#RSS_FEEDS_XML = os.path.join( CWD, "resources", "RssFeeds.xml" )

#BASE_THUMBS_PATH = os.path.join( sys.modules[ "__main__" ].SPECIAL_SCRIPT_DATA, "Thumbnails" )
MEDIA_PATH   = sys.modules[ "__main__" ].MEDIA_PATH
__language__ = sys.modules[ "__main__" ].__language__



class PluginMgr:
    """
    Provides plugin management tools
    """
    def parse_params( self ):
        """
        Parses Plugin parameters and returns it as a dictionary
        """
        paramDic={}
        # Parameters are on the 3rd arg passed to the script
        paramStr=sys.argv[2]
        if len(paramStr)>1:
            paramStr = paramStr.replace('?','')

            # Ignore last char if it is a '/'
            if (paramStr[len(paramStr)-1]=='/'):
                paramStr=paramStr[0:len(paramStr)-2]

            # Processing each parameter splited on  '&'
            for param in paramStr.split("&"):
                try:
                    # Spliting couple key/value
                    key,value=param.split("=")
                except:
                    key=param
                    value=""

                key = urllib.unquote_plus(key)
                value = urllib.unquote_plus(value)

                # Filling dictionnary
                paramDic[key]=value
        return paramDic

    def create_param_url(self, paramsDic):
        """
        Create an plugin URL based on the key/value passed in a dictionary
        """
        url = sys.argv[ 0 ]
        sep = '?'
        print paramsDic
        try:
            for param in paramsDic:
                #TODO: solve error on name with non ascii char (generate exception)
                url = url + sep + urllib.quote_plus( param ) + '=' + urllib.quote_plus( paramsDic[param] )
                sep = '&'
        except:
            url = None
            print_exc()
        return url

    def addLink( self, name, url, iconimage="DefaultProgram.png" ):
        ok=True
        print "_addLink"
        #TODO: reenable image downlaod, the checkURL freeze for whetever reason
        #if ( ( iconimage !="DefaultProgram.png" ) and ( not checkURL(iconimage) ) ):
        #    iconimage  = "DefaultProgram.png"

        print "Icon: %s"%iconimage
        liz=xbmcgui.ListItem( name, iconImage=iconimage, thumbnailImage=iconimage )
        print "List item created"
        liz.setInfo( type="Program", infoLabels={ "Title": name } )
        print "List item Set Info done"
        ok=xbmcplugin.addDirectoryItem( handle=int(sys.argv[1]), url=url, listitem=liz )
        print "Item added"
        return ok


    def addItemLink( self, itemInfo ):
        """
        Add a link to the list of items
        """
        ok=True

        print itemInfo

        if itemInfo["ImageUrl"]:
            icon = itemInfo["ImageUrl"]
        else:
            #icon = "DefaultFolder.png"
            #icon = "DefaultAddon.png"
            icon = os.path.join(MEDIA_PATH, "DefaultAddonRepository.png")

        if itemInfo["version"]:
            labelTxt = itemInfo["name"] + ((" ("+ itemInfo['version'] + ")") or "")
        else:
            labelTxt = itemInfo["name"]
#        descriptColor = self.colorList[ int( __settings__.getSetting( "descolor" ) ) ]
#        if self.shortTitleDisplay:
#            labelTxt = itemInfo["name"]
#        else:
#            labelTxt = itemInfo["name"] + ": " + self._coloring( itemInfo["description"], descriptColor )
        liz=xbmcgui.ListItem( label=labelTxt, iconImage=icon, thumbnailImage=icon )
        liz.setInfo( type="addons",
                     infoLabels={ "title": itemInfo["name"], "Plot": itemInfo["description"] } )
        liz.setProperty("Addon.Name", itemInfo["name"])
        liz.setProperty("Addon.Version", itemInfo["version"] or "")
        liz.setProperty("Addon.Summary", "")
        liz.setProperty("Addon.Description", itemInfo["description"] or "")
        liz.setProperty("Addon.Type", itemInfo[ "type" ]) #TODO: create localized string base on the type
        liz.setProperty("Addon.Creator", itemInfo["author"]  or "")
        liz.setProperty("Addon.Disclaimer", "")
        liz.setProperty("Addon.Changelog", "")
        liz.setProperty("Addon.ID", "")
        liz.setProperty("Addon.Status", "Stable")
        liz.setProperty("Addon.Broken", "Stable")
        liz.setProperty("Addon.Path","")
        liz.setProperty("Addon.Icon",icon)

        paramsMenu = {}
        paramsMenu[PARAM_NAME] = itemInfo["name"]
        paramsMenu[PARAM_ACTION] = VALUE_DISPLAY_INFO
        urlMenu = self.create_param_url( paramsMenu )
        if urlMenu:
            c_items = [ ( __language__( 30300 ), "XBMC.RunPlugin(%s)" % ( urlMenu)) ]
            liz.addContextMenuItems( c_items )
        urlItem = itemInfo["PluginUrl"]
        if urlItem:
            ok=xbmcplugin.addDirectoryItem( handle=int(sys.argv[1]), url=urlItem, listitem=liz, isFolder=False  )
        return ok


    def addDir( self, name, url, iconimage="DefaultFolder.png", c_items = None ):
        """
        Credit to ppic
        """
        print name
        print url
        print iconimage
        #u=sys.argv[0]+"?url="+urllib.quote_plus(url)+"&mode="+str(mode)+"&name="+urllib.quote_plus(name)
        ok=True
        liz=xbmcgui.ListItem( name, iconImage=iconimage, thumbnailImage=iconimage )
        if c_items :
            liz.addContextMenuItems( c_items, replaceItems=True )
        liz.setInfo( type="Program", infoLabels={ "Title": name } )
        ok=xbmcplugin.addDirectoryItem( handle=int( sys.argv[1] ), url=url, listitem=liz, isFolder=True )
        return ok


    def end_of_directory( self, OK, update=False ):
        xbmcplugin.endOfDirectory( handle=int( sys.argv[ 1 ] ), succeeded=OK, updateListing=update )#, cacheToDisc=True )#updateListing = True,

    def add_sort_methods( self, OK ):
        if ( OK ):
            try:
                xbmcplugin.addSortMethod( handle=int( sys.argv[ 1 ] ), sortMethod=xbmcplugin.SORT_METHOD_LABEL )
                #xbmcplugin.addSortMethod( handle=int( sys.argv[ 1 ] ), sortMethod=xbmcplugin.SORT_METHOD_LABEL )
                #xbmcplugin.addSortMethod( handle=int( sys.argv[ 1 ] ), sortMethod=xbmcplugin.SORT_METHOD_DATE )
            except:
                print_exc()

    def coloring( self, text , color , colorword ):
        if color == "red": color="FFFF0000"
        if color == "green": color="ff00FF00"
        if color == "yellow": color="ffFFFF00"
        colored_text = text.replace( colorword , "[COLOR=%s]%s[/COLOR]" % ( color , colorword ) )
        return colored_text

