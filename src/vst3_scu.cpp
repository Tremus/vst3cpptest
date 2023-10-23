#include <base/source/baseiids.cpp>
#include <base/source/fbuffer.cpp>
#include <base/source/fdebug.cpp>
#include <base/source/fdynlib.cpp>
#include <base/source/fobject.cpp>
#include <base/source/fstreamer.cpp>
#include <base/source/fstring.cpp>
#include <base/source/timer.cpp>
#include <base/source/updatehandler.cpp>
#include <base/thread/source/flock.cpp>

#include <pluginterfaces/base/coreiids.cpp>
#include <pluginterfaces/base/funknown.cpp>
#include <pluginterfaces/base/ustring.cpp>
#include <pluginterfaces/gui/iplugview.h>

#include <public.sdk/source/common/commoniids.cpp>
#include <public.sdk/source/common/pluginview.cpp>

#include <public.sdk/source/main/moduleinit.cpp>
#include <public.sdk/source/main/pluginfactory.cpp>

#include <public.sdk/source/vst/vstaudioeffect.cpp>
#include <public.sdk/source/vst/vstbus.cpp>
#include <public.sdk/source/vst/vstcomponent.cpp>
#include <public.sdk/source/vst/vstcomponentbase.cpp>
#include <public.sdk/source/vst/vsteditcontroller.cpp>
#include <public.sdk/source/vst/vstinitiids.cpp>
#include <public.sdk/source/vst/vstparameters.cpp>

#ifdef __APPLE__
#include <public.sdk/source/main/macmain.cpp>
#endif