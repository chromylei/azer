{
  'variables': {
    'windows_sdk_path%':'D:/Program Files (x86)/Windows Kits/8.0',
    'd3d_sdk_path%':'D:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/',
    'glew_sdk_path%':'D:/cygwin64/root/home/Administrator/opengl/third_party/glew-1.10.0/',
    'glut_sdk_path%':'D:/cygwin64/root/home/Administrator/opengl/third_party/glut-3.7.6-bin',
    'win_opt_rt_library%': '2',  # 2 = /MD
    'win_dbg_rt_library%': '3', # 3 = /MDd
    'win_opt_rt_check%': '0',
    'win_dbg_rt_check%': '0',
    'msvs_dbg_link_incremental': '2',
    'msvs_opt_link_incremental': '2',
    'win_dbg_optimization%': '0',
    'win_opt_optimization%': '2',
  },

  'target_defaults': {
    'default_configuration': 'dbg',
    'configurations': {
      'base': {
        'abstract': 1,
        'msvs_configuration_attributes': {
          'OutputDirectory': '<(DEPTH)\\build\\out\\$(ConfigurationName)',
          'IntermediateDirectory': '$(OutDir)\\obj\\$(ProjectName)',
          'CharacterSet': '1',
        },

        'msvs_system_include_dirs': [
          '<(windows_sdk_path)/Include/shared',
          '<(windows_sdk_path)/Include/um',
          '<(windows_sdk_path)/Include/winrt',
          '<(d3d_sdk_path)/Include',
          '<(glew_sdk_path)/include',
          '<(glut_sdk_path)',
          '<(DEPTH)/',
        ],

        'msvs_settings':{
          'VCCLCompilerTool': {
            'UseFullPaths':'false',
          },
          
          'VCLinkerTool': {
            'AdditionalLibraryDirectories': [
              '<(windows_sdk_path)/Lib/win8/um/x86',
              '<(d3d_sdk_path)/Lib/x86',
              '<(glew_sdk_path)/lib/Release/Win32',
              '<(glut_sdk_path)',
            ],

            'AdditionalDependencies': [
              'kernel32.lib',
              'gdi32.lib',
              'winspool.lib',
              'Winmm.lib',
              'comdlg32.lib',
              'advapi32.lib',
              'shell32.lib',
              'ole32.lib',
              'oleaut32.lib',
              'user32.lib',
              'uuid.lib',
              'odbc32.lib',
              'odbccp32.lib',
              'delayimp.lib',
              'comctl32.lib',
              'glew32.lib',
              'glut32.lib',
            ],
            'TargetMachine': '1',  # x86
          },

	   'VCManifestTool': {
	     'EmbedManifest': 'false',
	   },
        },
      },  # 'base'
      
      'dbg_base': {
        'abstract': 1,
        'msvs_settings': {
          'VCCLCompilerTool': {
            'PreprocessorDefinitions': ['_DEBUG'],
            'BasicRuntimeChecks': '<(win_dbg_rt_check)',
            'RuntimeLibrary': '<(win_dbg_rt_library)',
	    'Optimization': '<(win_dbg_optimization)',
          },
          
          'VCLinkerTool': {
            'LinkIncremental': '<(msvs_dbg_link_incremental)',
            'RandomizedBaseAddress': 1,
            'GenerateDebugInformation': 'true',
          },
          
          'VCResourceCompilerTool': {
            'PreprocessorDefinitions': ['_DEBUG'],
          },
        },
      }, # dbg_base

      'opt_base': {
        'abstract': 1,
        'msvs_settings': {
          'VCCLCompilerTool': {
            'PreprocessorDefinitions': ['NDEBUG'],
            'BasicRuntimeChecks': '<(win_opt_rt_check)',
            'RuntimeLibrary': '<(win_opt_rt_library)',
	    'Optimization': '<(win_opt_optimization)',
          },
          
          'VCLinkerTool': {
            'LinkIncremental': '<(msvs_opt_link_incremental)',
            'RandomizedBaseAddress': 0,
            'GenerateDebugInformation': 'false',
          },
          
          'VCResourceCompilerTool': {
            'PreprocessorDefinitions': ['NDEBUG'],
          },
        },
      }, # dbg_base
      
      'dbg': {
        'inherit_from': ['base', 'dbg_base'],
      },
      'opt': {
        'inherit_from': ['base', 'opt_base'],
      },
    },  # configuration
  }, # default
}
