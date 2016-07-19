/******************************************************************
 *
 * IoTivity secure thermostat. Derived from SampleSecureServer.
 *
 ******************************************************************/


/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "RCSResourceObject.h"
#include "OCPlatform.h"


using namespace OC::OCPlatform;
using namespace OIC::Service;

struct CloseApp {};

constexpr int INCREASE = 1;
constexpr int DECREASE = 2;

typedef void (*DisplayControlMenuFunc)();
typedef std::function<void()> Run;

Run g_currentRun;
bool g_isPresenceStarted = false;

RCSResourceObject::Ptr g_resource0;
RCSResourceObject::Ptr g_resource1;
RCSResourceObject::Ptr g_resource2;

int processUserInput(int min, int max) {
  assert(min <= max);
  int input;

  std::cin >> input;

  if (!std::cin.fail()) {
    if (input == max + 1) throw CloseApp();
    if (min <= input) return input;
  }

  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  throw std::runtime_error("Invalid Input, please try again");
}


void displayControlLightMenu() {
  std::cout << "========================================================\n";
  std::cout << INCREASE << ". Increase temperature\n";
  std::cout << DECREASE << ". Decrease temperature\n";
  std::cout << DECREASE + 1 << ". Quit\n";
  std::cout << "========================================================\n";
}


void printAttributes(const RCSResourceAttributes &attrs) {
  for (const auto &attr : attrs) {
    std::cout << "\tkey : " << attr.key() << "\n\tvalue : "
          << attr.value().toString() << std::endl;
  }
}


RCSGetResponse getHandler1(const RCSRequest &, RCSResourceAttributes &attrs) {
  printAttributes(attrs);
  RCSResourceObject::LockGuard lock(g_resource1);
  std::cout << "\nSending response to Client : " << std::endl;
  printAttributes(g_resource1->getAttributes());
  return RCSGetResponse::defaultAction();
}

RCSSetResponse setHandler1(const RCSRequest &, RCSResourceAttributes &attrs) {
  printAttributes(attrs);
  return RCSSetResponse::defaultAction();
}

RCSGetResponse getHandler2(const RCSRequest &, RCSResourceAttributes &attrs) {
  printAttributes(attrs);
  RCSResourceObject::LockGuard lock(g_resource2);
  std::cout << "\nSending response to Client : " << std::endl;
  printAttributes(g_resource2->getAttributes());
  return RCSGetResponse::defaultAction();
}

RCSSetResponse setHandler2(const RCSRequest &, RCSResourceAttributes &attrs) {
  printAttributes(attrs);
  return RCSSetResponse::defaultAction();
}



RCSGetResponse getHandler0(const RCSRequest &, RCSResourceAttributes &attrs) {
  printAttributes(attrs);

  RCSResourceObject::LockGuard lock(g_resource0);
  std::cout << "\nSending response to Client : " << std::endl;
  printAttributes(g_resource0->getAttributes());
  return RCSGetResponse::defaultAction();
}


void initServer(RCSResourceObject::Ptr* _resource,
                const std::string &resourceUri,
                const std::string &resourceType,
                const std::string &attrKey)
{
  RCSResourceObject::Ptr g_resource = RCSResourceObject::Builder(resourceUri, resourceType, "oic.if.baseline").
         setDiscoverable(true).setObservable(true).setSecureFlag(true).build();

  g_resource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::UPDATED);
  g_resource->setSetRequestHandlerPolicy(RCSResourceObject::SetRequestHandlerPolicy::NEVER);
  g_resource->setAttribute(attrKey, 0);

  *(_resource) = g_resource;
}


void updateAttribute(const std::string &attrKey, int control) {
  const int diff = control == INCREASE ? 1 : - 1;

  {
    RCSResourceObject::LockGuard lock(g_resource0);
    auto &attrs = g_resource0->getAttributes();
    attrs[attrKey] = attrs[attrKey].get<int>() + diff;
  }

  if (control == INCREASE) {
    std::cout << attrKey << " increased." << std::endl;
  }
  else {
    std::cout << attrKey << " decreased." << std::endl;
  }
  std::cout << "\nCurrent " << attrKey << ": "
        << g_resource0->getAttributeValue(attrKey).get<int>() << std::endl;
}


void runResourceControl(DisplayControlMenuFunc displayMenuFunc, const std::string &attrKey) {
  displayMenuFunc();
  updateAttribute(attrKey, processUserInput(INCREASE, DECREASE));
}


static FILE *client_open(const char * /*path*/, const char *mode) {
    return fopen("./thermostat_db.cbor", mode);
}


int main(void) {
  OCPersistentStorage ps {
    client_open,
    fread,
    fwrite,
    fclose,
    unlink
  };

  OC::PlatformConfig cfg {
    OC::ServiceType::InProc, OC::ModeType::Server, "0.0.0.0", 0,
    OC::QualityOfService::LowQos, &ps
  };

  OC::OCPlatform::Configure(cfg);

  // Start announcing ourselves.
  g_isPresenceStarted = true;
  startPresence(3);

  std::string attrKey0 = "CurrentTemperature";
  std::string attrKey1 = "DesiredTemperature";
  std::string attrKey2 = "FanSetting";
  initServer(&g_resource0, "/a/temperature", "oic.r.temperature",   attrKey0);
  initServer(&g_resource1, "/a/desiredTemp", "oic.r.temperature",   attrKey1);
  initServer(&g_resource2, "/a/fan",         "oic.r.switch.binary", attrKey2);

  g_resource0->setGetRequestHandler(getHandler0);
  g_resource1->setGetRequestHandler(getHandler1);
  g_resource2->setGetRequestHandler(getHandler2);

  g_resource1->setSetRequestHandler(setHandler1);
  g_resource2->setSetRequestHandler(setHandler2);

  g_currentRun = std::bind(runResourceControl, displayControlLightMenu, std::move(attrKey1));


  while (true) {
    try {
      g_currentRun();
    }
    catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
    }
    catch (const CloseApp &) {
      break;
    }
  }
  std::cout << "Stopping the server" << std::endl;

  g_resource0.reset();
  g_resource1.reset();
  g_resource2.reset();

  if (g_isPresenceStarted) {
    stopPresence();
  }
}
