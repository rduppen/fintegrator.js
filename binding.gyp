{
   "variables": {
    "ws_root%" : "/sep/workspace",
	"ace_root%" : "/srv/fgi/ACE_wrappers/",
    "tao_root%" : "/srv/fgi/ACE_wrappers/TAO",
    "fgi_root%" : "/sep/workspace/interfaces/cmf",
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
        "<!(node -e \"require('nan')\")",
        "include",
        "<@(ace_root)", 
        "<@(tao_root)",
        "<@(tao_root)/tao",
        "<@(tao_root)/orbsvcs",
        "<@(tao_root)/orbsvcs/orbsvcs",
        "../include/<@(os)",        
        "<@(fgi_root)/include",
        "<@(fgi_root)/include/<@(os)", 
        "<@(ws_root)/src/tools/include" ],
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
          "ACE_HAS_SVCCONF",
          'x86_64_pc_linux_rhel6',
          'IP_ORB_TAO'
        ],
      "libraries": [ 
        "-lACE", "-lTAO", "-lTAO_CosNaming", "-lTAO_Messaging", 
        "-lTAO_Svc_Utils", "-LTAO_IORTable", "-lTAO_PortableServer", "-lTAO_Valuetype", "-lTAO_Security",
        "-L<@(ace_root)/lib", 
        "-lxerces-c.27", "-L<@(ws_root)/lib/xml/2_7_0/lib/<@(os)",
        "-lcmf<@(bin_ext)", "-lcorba_utils<@(bin_ext)", "-L<@(fgi_root)/lib/<@(os)",        
        "-ldebug<@(bin_ext)", "-ldebug_ld<@(bin_ext)", "-L<@(ws_root)/src/tools/lib/<@(os)",
        "-lmsg<@(bin_ext)", "-L<@(ws_root)/src/msg/lib/<@(os)"
       ]
    }
  ]
}
