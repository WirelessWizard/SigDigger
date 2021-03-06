//
//    AppConfig.h: Application configuration object
//    Copyright (C) 2019 Gonzalo José Carracedo Carballal
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this program.  If not, see
//    <http://www.gnu.org/licenses/>
//

#include "AppConfig.h"

using namespace SigDigger;

AppConfig::AppConfig(AppUI *ui)
{
  this->sourceConfig    = ui->sourcePanel->getConfig();
  this->fftConfig       = ui->fftPanel->getConfig();
  this->inspectorConfig = ui->inspectorPanel->getConfig();
  this->audioConfig     = ui->audioPanel->getConfig();
}

Suscan::Object &&
AppConfig::serialize(void)
{
  Suscan::Object profileObj = this->profile.serialize();
  Suscan::Object obj(SUSCAN_OBJECT_TYPE_OBJECT);

  obj.setClass("qtui");

  obj.set("width", this->width);
  obj.set("height", this->height);
  obj.set("x", this->x);
  obj.set("y", this->y);
  obj.set("loFreq", this->loFreq);
  obj.set("bandwidth", this->bandwidth);

  obj.setField("source", profileObj);
  obj.setField("analyzerParams", this->analyzerParams.serialize());
  obj.setField("colors", this->colors.serialize());
  obj.setField("sourcePanel", this->sourceConfig->serialize());
  obj.setField("fftPanel", this->fftConfig->serialize());
  obj.setField("audioPanel", this->audioConfig->serialize());
  obj.setField("inspectorPanel", this->inspectorConfig->serialize());

  // Welcome to the world of stupid C++ hacks
  return this->persist(obj);
}

void
AppConfig::loadDefaults(void)
{
  this->profile = Suscan::Source::Config(
        SUSCAN_SOURCE_TYPE_SDR,
        SUSCAN_SOURCE_FORMAT_AUTO);
}

#define TRYSILENT(x) \
  try { x; } catch (Suscan::Exception const &e) { printf ("Error: %s\n", e.what()); }
void
AppConfig::deserialize(Suscan::Object const &conf)
{
  this->loadDefaults();
  if (!conf.isHollow()) {
    TRYSILENT(this->profile = Suscan::Source::Config(conf.getField("source")));
    TRYSILENT(this->analyzerParams.deserialize(conf.getField("analyzerParams")));
    TRYSILENT(this->colors.deserialize(conf.getField("colors")));
    TRYSILENT(this->sourceConfig->deserialize(conf.getField("sourcePanel")));
    TRYSILENT(this->fftConfig->deserialize(conf.getField("fftPanel")));
    TRYSILENT(this->audioConfig->deserialize(conf.getField("audioPanel")));
    TRYSILENT(this->inspectorConfig->deserialize(conf.getField("inspectorPanel")));

    TRYSILENT(this->width  = conf.get("width", this->width));
    TRYSILENT(this->height = conf.get("height", this->height));
    TRYSILENT(this->x      = conf.get("x", this->x));
    TRYSILENT(this->y      = conf.get("y", this->y));
    TRYSILENT(this->loFreq = conf.get("loFreq", this->loFreq));
    TRYSILENT(this->bandwidth = conf.get("bandwidth", this->bandwidth));
  }
}

// OH MY GOD HOLY CHRIST
[[ noreturn ]]
AppConfig::AppConfig(const Suscan::Object &) : AppConfig()
{
  throw Suscan::Exception("Improper call to AppConfig constructor");
}

