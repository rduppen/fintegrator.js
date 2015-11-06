{
   "variables": {
    "ws_root%" : "/home/able",
    "local%" : "/home/able/local",
    "linclude%" : "/home/able/local/include",
    "fgi_root%" : "/home/able/cmf",
    "os%" : "x86_64-pc-linux-rhel6",
    "bin_ext%": "msd"    
   },
  "targets": [
    {
      "target_name": "fintegrator",
      "sources": [ 
        "c/fintegrator.cpp", 
        "c/fgi_nodejs_Connection.cpp", 
        "c/fgi_nodejs_Session.cpp", 
        "c/fgi_nodejs_MessageProducer.cpp",
        "c/fgi_nodejs_MessageConsumer.cpp",
        "c/fgi_nodejs_Message.cpp" ], 
      "include_dirs" : [ 
        "include",
        "<!(nodejs -e \"require('nan')\")",
        "<@(local)/include", 
        "<@(local)/include/tao",
        "<@(local)/include/tao/orbsvcs",
        "<@(local)/include/tao/orbsvcs/orbsvcs",
        "../include/<@(os)",        
        "<@(fgi_root)/include",
        "<@(fgi_root)/include/<@(os)", 
        "<@(ws_root)/tools/include" ],
      "cflags" : [ 
        "-fexceptions",
        "-W",
        "-Wall",
        "-Wpointer-arith",
        "-pipe",
        "-g",
        "-Wno-reorder",
        "-Wno-deprecated",
        "-fPIC",
        "-shared", 
        "-funsigned-char",
        "-fpermissive",
        "-Wa,-Z"    ],
      'cflags!': [ '-fno-exceptions', '-fno-rtti' ],
      'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],      
      'defines': [
          'OS=<@(os)',
          'UNIX',
          'LINUX',
          '_REENTRANT',
          '__STDC_LIMIT_MACROS',
          "ACE_HAS_AIO_CALLS",
          "ACE_HAS_EXCEPTIONS", 
          "__ACE_INLINE__",
          "ACE_HAS_THREADS",          
          'x86_64_pc_linux_rhel6',
          'IP_ORB_TAO'
        ],
      "libraries": [ 
        "-lACE", "-lTAO", "-lTAO_CosNaming", "-lTAO_Messaging", 
        "-lTAO_Svc_Utils", "-LTAO_IORTable", "-lTAO_PortableServer", "-lTAO_Valuetype", "-lTAO_Security",
        "-L<@(local)/lib", 
        "-lxerces-c",
        "-lcmf<@(bin_ext)", "-lcorba_utils<@(bin_ext)", "-L<@(local)/lib/fgi",        
        "-ldebug<@(bin_ext)", "-ldebug_ld<@(bin_ext)",
        "-lmsg<@(bin_ext)"
       ]
    }
  ]
}
