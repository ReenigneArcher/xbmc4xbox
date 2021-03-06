

#import os
#import urllib
import sys
#import xbmc
#import xbmcplugin
#import xbmcgui
from traceback import print_exc

#__script__       = sys.modules[ "__main__" ].__script__
#__plugin__       = sys.modules[ "__main__" ].__plugin__
#__author__       = sys.modules[ "__main__" ].__author__
#__url__          = sys.modules[ "__main__" ].__url__
#__svn_url__      = sys.modules[ "__main__" ].__svn_url__
#__credits__      = sys.modules[ "__main__" ].__credits__
#__platform__     = sys.modules[ "__main__" ].__platform__
#__date__         = sys.modules[ "__main__" ].__date__
#__version__      = sys.modules[ "__main__" ].__version__
#__svn_revision__ = sys.modules[ "__main__" ].__svn_revision__
#__XBMC_Revision__= sys.modules[ "__main__" ].__XBMC_Revision__
__language__     = sys.modules[ "__main__" ].__language__
#ROOTDIR            = sys.modules[ "__main__" ].ROOTDIR
#BASE_RESOURCE_PATH = sys.modules[ "__main__" ].BASE_RESOURCE_PATH
#LIBS_PATH          = sys.modules[ "__main__" ].LIBS_PATH
#MEDIA_PATH         = sys.modules[ "__main__" ].MEDIA_PATH


# Custom modules
try:
    from globalvars import PARAM_REPO_ID, PARAM_LISTTYPE, PARAM_TYPE, VALUE_LIST_ADDONS, VALUE_LIST_ALL_ADDONS
    from PluginMgr import PluginMgr
    from Item import TYPE_ADDON_SCRIPT, TYPE_ADDON_MUSIC, TYPE_ADDON_PICTURES, TYPE_ADDON_PROGRAMS, TYPE_ADDON_VIDEO, TYPE_ADDON_WEATHER, TYPE_ADDON_MODULE
except:
    print_exc()


class Main:
    """
    Main plugin class
    """

    def __init__( self, *args, **kwargs ):

        self.pluginMgr = PluginMgr()
        self.parameters = self.pluginMgr.parse_params()

        print "List of Add-ons' categories"
        repoId = self.parameters[ PARAM_REPO_ID ]
        self._createAddonCatDir( repoId )

        self.pluginMgr.add_sort_methods( True )
        self.pluginMgr.end_of_directory( True, update=False )


    def _createAddonCatDir ( self, repoId ):
        """
        Creates list of addon categories for a specific repository
        """
        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = VALUE_LIST_ALL_ADDONS
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30107 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_SCRIPT
        url = self.pluginMgr.create_param_url( params )
        self.pluginMgr.addDir( __language__( 30101 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_MUSIC
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30102 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_PICTURES
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30103 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_PROGRAMS
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30104 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_VIDEO
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30105 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_WEATHER
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30106 ), url )

        params = {}
        params[PARAM_REPO_ID] = str(repoId)
        params[PARAM_LISTTYPE] = VALUE_LIST_ADDONS
        params[PARAM_TYPE] = TYPE_ADDON_MODULE
        url = self.pluginMgr.create_param_url( params )
        if url:
            self.pluginMgr.addDir( __language__( 30108 ), url )
