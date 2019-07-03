#include "ResTableConfig.h"

 bool matchConfig(const ResTable_config& settings, ResTable_config& o)  {
    if (o.imsi != 0) {
        if (o.mcc != 0 && o.mcc != settings.mcc) {
            return false;
        }
        if (o.mnc != 0 && o.mnc != settings.mnc) {
            return false;
        }
    }
    if (o.locale != 0) {
        if (o.language[0] != 0
            && (o.language[0] != settings.language[0]
                || o.language[1] != settings.language[1])) {
            return false;
        }
        if (o.country[0] != 0
            && (o.country[0] != settings.country[0]
                || o.country[1] != settings.country[1])) {
            return false;
        }
    }
    if (o.screenConfig != 0) {
        const int layoutDir = o.screenLayout&MASK_LAYOUTDIR;
        const int setLayoutDir = settings.screenLayout&MASK_LAYOUTDIR;
        if (layoutDir != 0 && layoutDir != setLayoutDir) {
            return false;
        }

        const int screenSize = o.screenLayout&MASK_SCREENSIZE;
        const int setScreenSize = settings.screenLayout&MASK_SCREENSIZE;
        // Any screen sizes for larger screens than the setting do not
        // match.
        if (screenSize != 0 && screenSize > setScreenSize) {
            return false;
        }

        const int screenLong = o.screenLayout&MASK_SCREENLONG;
        const int setScreenLong = settings.screenLayout&MASK_SCREENLONG;
        if (screenLong != 0 && screenLong != setScreenLong) {
            return false;
        }

        const int uiModeType = o.uiMode&MASK_UI_MODE_TYPE;
        const int setUiModeType = settings.uiMode&MASK_UI_MODE_TYPE;
        if (uiModeType != 0 && uiModeType != setUiModeType) {
            return false;
        }

        const int uiModeNight = o.uiMode&MASK_UI_MODE_NIGHT;
        const int setUiModeNight = settings.uiMode&MASK_UI_MODE_NIGHT;
        if (uiModeNight != 0 && uiModeNight != setUiModeNight) {
            return false;
        }

        if (o.smallestScreenWidthDp != 0
                && o.smallestScreenWidthDp > settings.smallestScreenWidthDp) {
            return false;
        }
    }
    if (o.screenSizeDp != 0) {
        if (o.screenWidthDp != 0 && o.screenWidthDp > settings.screenWidthDp) {
            //ALOGI("Filtering out width %d in requested %d", screenWidthDp, settings.screenWidthDp);
            return false;
        }
        if (o.screenHeightDp != 0 && o.screenHeightDp > settings.screenHeightDp) {
            //ALOGI("Filtering out height %d in requested %d", screenHeightDp, settings.screenHeightDp);
            return false;
        }
    }
    if (o.screenType != 0) {
        if (o.orientation != 0 && o.orientation != settings.orientation) {
            return false;
        }
        // density always matches - we can scale it.  See isBetterThan
        if (o.touchscreen != 0 && o.touchscreen != settings.touchscreen) {
            return false;
        }
    }
    if (o.input != 0) {
        const int keysHidden = o.inputFlags&MASK_KEYSHIDDEN;
        const int setKeysHidden = settings.inputFlags&MASK_KEYSHIDDEN;
        if (keysHidden != 0 && keysHidden != setKeysHidden) {
            // For compatibility, we count a request for KEYSHIDDEN_NO as also
            // matching the more recent KEYSHIDDEN_SOFT.  Basically
            // KEYSHIDDEN_NO means there is some kind of keyboard available.
            //ALOGI("Matching keysHidden: have=%d, config=%d\n", keysHidden, setKeysHidden);
            if (keysHidden != KEYSHIDDEN_NO || setKeysHidden != KEYSHIDDEN_SOFT) {
                //ALOGI("No match!");
                return false;
            }
        }
        const int navHidden = o.inputFlags&MASK_NAVHIDDEN;
        const int setNavHidden = settings.inputFlags&MASK_NAVHIDDEN;
        if (navHidden != 0 && navHidden != setNavHidden) {
            return false;
        }
        if (o.keyboard != 0 && o.keyboard != settings.keyboard) {
            return false;
        }
        if (o.navigation != 0 && o.navigation != settings.navigation) {
            return false;
        }
    }
    if (o.screenSize != 0) {
        if (o.screenWidth != 0 && o.screenWidth > settings.screenWidth) {
            return false;
        }
        if (o.screenHeight != 0 && o.screenHeight > settings.screenHeight) {
            return false;
        }
    }
    if (o.version != 0) {
        if (o.sdkVersion != 0 && o.sdkVersion > settings.sdkVersion) {
            return false;
        }
        if (o.minorVersion != 0 && o.minorVersion != settings.minorVersion) {
            return false;
        }
    }
    return true;
}

void getConfigLocale(char str[6], ResTable_config& o) {
    memset(str, 0, 6);
    if (o.language[0]) {
        str[0] = o.language[0];
        str[1] = o.language[1];
        if (o.country[0]) {
            str[2] = '_';
            str[3] = o.country[0];
            str[4] = o.country[1];
        }
    }
}

String8 configToString(const ResTable_config& o) {
    String8 res;

    if (o.mcc != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dmcc", dtohs(o.mcc));
    }
    if (o.mnc != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dmnc", dtohs(o.mnc));
    }
    if (o.language[0] != 0) {
        if (res.size() > 0) res.append("-");
        res.append(o.language, 2);
    }
    if (o.country[0] != 0) {
        if (res.size() > 0) res.append("-");
        res.append(o.country, 2);
    }
    if ((o.screenLayout&MASK_LAYOUTDIR) != 0) {
        if (res.size() > 0) res.append("-");
        switch (o.screenLayout&MASK_LAYOUTDIR) {
            case LAYOUTDIR_LTR:
                res.append("ldltr");
                break;
            case LAYOUTDIR_RTL:
                res.append("ldrtl");
                break;
            default:
                res.appendFormat("layoutDir=%d",
                        dtohs(o.screenLayout&MASK_LAYOUTDIR));
                break;
        }
    }
    if (o.smallestScreenWidthDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("sw%ddp", dtohs(o.smallestScreenWidthDp));
    }
    if (o.screenWidthDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("w%ddp", dtohs(o.screenWidthDp));
    }
    if (o.screenHeightDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("h%ddp", dtohs(o.screenHeightDp));
    }
    if ((o.screenLayout&MASK_SCREENSIZE) != SCREENSIZE_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.screenLayout&MASK_SCREENSIZE) {
            case SCREENSIZE_SMALL:
                res.append("small");
                break;
            case SCREENSIZE_NORMAL:
                res.append("normal");
                break;
            case SCREENSIZE_LARGE:
                res.append("large");
                break;
            case SCREENSIZE_XLARGE:
                res.append("xlarge");
                break;
            default:
                res.appendFormat("screenLayoutSize=%d",
                        dtohs(o.screenLayout&MASK_SCREENSIZE));
                break;
        }
    }
    if ((o.screenLayout&MASK_SCREENLONG) != 0) {
        if (res.size() > 0) res.append("-");
        switch (o.screenLayout&MASK_SCREENLONG) {
            case SCREENLONG_NO:
                res.append("notlong");
                break;
            case SCREENLONG_YES:
                res.append("long");
                break;
            default:
                res.appendFormat("screenLayoutLong=%d",
                        dtohs(o.screenLayout&MASK_SCREENLONG));
                break;
        }
    }
    if (o.orientation != ORIENTATION_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.orientation) {
            case ORIENTATION_PORT:
                res.append("port");
                break;
            case ORIENTATION_LAND:
                res.append("land");
                break;
            case ORIENTATION_SQUARE:
                res.append("square");
                break;
            default:
                res.appendFormat("orientation=%d", dtohs(o.orientation));
                break;
        }
    }
    if ((o.uiMode&MASK_UI_MODE_TYPE) != UI_MODE_TYPE_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.uiMode&MASK_UI_MODE_TYPE) {
            case UI_MODE_TYPE_DESK:
                res.append("desk");
                break;
            case UI_MODE_TYPE_CAR:
                res.append("car");
                break;
            case UI_MODE_TYPE_TELEVISION:
                res.append("television");
                break;
            case UI_MODE_TYPE_APPLIANCE:
                res.append("appliance");
                break;
            default:
                res.appendFormat("uiModeType=%d",
                        dtohs(o.screenLayout&MASK_UI_MODE_TYPE));
                break;
        }
    }
    if ((o.uiMode&MASK_UI_MODE_NIGHT) != 0) {
        if (res.size() > 0) res.append("-");
        switch (o.uiMode&MASK_UI_MODE_NIGHT) {
            case UI_MODE_NIGHT_NO:
                res.append("notnight");
                break;
            case UI_MODE_NIGHT_YES:
                res.append("night");
                break;
            default:
                res.appendFormat("uiModeNight=%d",
                        dtohs(o.uiMode&MASK_UI_MODE_NIGHT));
                break;
        }
    }
    if (o.density != DENSITY_DEFAULT) {
        if (res.size() > 0) res.append("-");
        switch (o.density) {
            case DENSITY_LOW:
                res.append("ldpi");
                break;
            case DENSITY_MEDIUM:
                res.append("mdpi");
                break;
            case DENSITY_TV:
                res.append("tvdpi");
                break;
            case DENSITY_HIGH:
                res.append("hdpi");
                break;
            case DENSITY_XHIGH:
                res.append("xhdpi");
                break;
            case DENSITY_XXHIGH:
                res.append("xxhdpi");
                break;
            case DENSITY_NONE:
                res.append("nodpi");
                break;
            default:
                res.appendFormat("%ddpi", dtohs(o.density));
                break;
        }
    }
    if (o.touchscreen != TOUCHSCREEN_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.touchscreen) {
            case TOUCHSCREEN_NOTOUCH:
                res.append("notouch");
                break;
            case TOUCHSCREEN_FINGER:
                res.append("finger");
                break;
            case TOUCHSCREEN_STYLUS:
                res.append("stylus");
                break;
            default:
                res.appendFormat("touchscreen=%d", dtohs(o.touchscreen));
                break;
        }
    }
    if (o.keyboard != KEYBOARD_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.keyboard) {
            case KEYBOARD_NOKEYS:
                res.append("nokeys");
                break;
            case KEYBOARD_QWERTY:
                res.append("qwerty");
                break;
            case KEYBOARD_12KEY:
                res.append("12key");
                break;
            default:
                res.appendFormat("keyboard=%d", dtohs(o.keyboard));
                break;
        }
    }
    if ((o.inputFlags&MASK_KEYSHIDDEN) != 0) {
        if (res.size() > 0) res.append("-");
        switch (o.inputFlags&MASK_KEYSHIDDEN) {
            case KEYSHIDDEN_NO:
                res.append("keysexposed");
                break;
            case KEYSHIDDEN_YES:
                res.append("keyshidden");
                break;
            case KEYSHIDDEN_SOFT:
                res.append("keyssoft");
                break;
        }
    }
    if (o.navigation != NAVIGATION_ANY) {
        if (res.size() > 0) res.append("-");
        switch (o.navigation) {
            case NAVIGATION_NONAV:
                res.append("nonav");
                break;
            case NAVIGATION_DPAD:
                res.append("dpad");
                break;
            case NAVIGATION_TRACKBALL:
                res.append("trackball");
                break;
            case NAVIGATION_WHEEL:
                res.append("wheel");
                break;
            default:
                res.appendFormat("navigation=%d", dtohs(o.navigation));
                break;
        }
    }
    if ((o.inputFlags&MASK_NAVHIDDEN) != 0) {
        if (res.size() > 0) res.append("-");
        switch (o.inputFlags&MASK_NAVHIDDEN) {
            case NAVHIDDEN_NO:
                res.append("navsexposed");
                break;
            case NAVHIDDEN_YES:
                res.append("navhidden");
                break;
            default:
                res.appendFormat("inputFlagsNavHidden=%d",
                        dtohs(o.inputFlags&MASK_NAVHIDDEN));
                break;
        }
    }
    if (o.screenSize != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dx%d", dtohs(o.screenWidth), dtohs(o.screenHeight));
    }
    if (o.version != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("v%d", dtohs(o.sdkVersion));
        if (o.minorVersion != 0) {
            res.appendFormat(".%d", dtohs(o.minorVersion));
        }
    }

    return res;
}

bool isMoreSpecificThan(const ResTable_config& o,  const ResTable_config& t) {
    // The order of the following tests defines the importance of one
    // configuration parameter over another.  Those tests first are more
    // important, trumping any values in those following them.
    if (t.imsi || o.imsi) {
        if (t.mcc != o.mcc) {
            if (!t.mcc) return false;
            if (!o.mcc) return true;
        }

        if (t.mnc != o.mnc) {
            if (!t.mnc) return false;
            if (!o.mnc) return true;
        }
    }

    if (t.locale || o.locale) {
        if (t.language[0] != o.language[0]) {
            if (!t.language[0]) return false;
            if (!o.language[0]) return true;
        }

        if (t.country[0] != o.country[0]) {
            if (!t.country[0]) return false;
            if (!o.country[0]) return true;
        }
    }

    if (t.screenLayout || o.screenLayout) {
        if (((t.screenLayout^o.screenLayout) & MASK_LAYOUTDIR) != 0) {
            if (!(t.screenLayout & MASK_LAYOUTDIR)) return false;
            if (!(o.screenLayout & MASK_LAYOUTDIR)) return true;
        }
    }

    if (t.smallestScreenWidthDp || o.smallestScreenWidthDp) {
        if (t.smallestScreenWidthDp != o.smallestScreenWidthDp) {
            if (!t.smallestScreenWidthDp) return false;
            if (!o.smallestScreenWidthDp) return true;
        }
    }

    if (t.screenSizeDp || o.screenSizeDp) {
        if (t.screenWidthDp != o.screenWidthDp) {
            if (!t.screenWidthDp) return false;
            if (!o.screenWidthDp) return true;
        }

        if (t.screenHeightDp != o.screenHeightDp) {
            if (!t.screenHeightDp) return false;
            if (!o.screenHeightDp) return true;
        }
    }

    if (t.screenLayout || o.screenLayout) {
        if (((t.screenLayout^o.screenLayout) & MASK_SCREENSIZE) != 0) {
            if (!(t.screenLayout & MASK_SCREENSIZE)) return false;
            if (!(o.screenLayout & MASK_SCREENSIZE)) return true;
        }
        if (((t.screenLayout^o.screenLayout) & MASK_SCREENLONG) != 0) {
            if (!(t.screenLayout & MASK_SCREENLONG)) return false;
            if (!(o.screenLayout & MASK_SCREENLONG)) return true;
        }
    }

    if (t.orientation != o.orientation) {
        if (!t.orientation) return false;
        if (!o.orientation) return true;
    }

    if (t.uiMode || o.uiMode) {
        if (((t.uiMode^o.uiMode) & MASK_UI_MODE_TYPE) != 0) {
            if (!(t.uiMode & MASK_UI_MODE_TYPE)) return false;
            if (!(o.uiMode & MASK_UI_MODE_TYPE)) return true;
        }
        if (((t.uiMode^o.uiMode) & MASK_UI_MODE_NIGHT) != 0) {
            if (!(t.uiMode & MASK_UI_MODE_NIGHT)) return false;
            if (!(o.uiMode & MASK_UI_MODE_NIGHT)) return true;
        }
    }

    // density is never 'more specific'
    // as the default just equals 160

    if (t.touchscreen != o.touchscreen) {
        if (!t.touchscreen) return false;
        if (!o.touchscreen) return true;
    }

    if (t.input || o.input) {
        if (((t.inputFlags^o.inputFlags) & MASK_KEYSHIDDEN) != 0) {
            if (!(t.inputFlags & MASK_KEYSHIDDEN)) return false;
            if (!(o.inputFlags & MASK_KEYSHIDDEN)) return true;
        }

        if (((t.inputFlags^o.inputFlags) & MASK_NAVHIDDEN) != 0) {
            if (!(t.inputFlags & MASK_NAVHIDDEN)) return false;
            if (!(o.inputFlags & MASK_NAVHIDDEN)) return true;
        }

        if (t.keyboard != o.keyboard) {
            if (!t.keyboard) return false;
            if (!o.keyboard) return true;
        }

        if (t.navigation != o.navigation) {
            if (!t.navigation) return false;
            if (!o.navigation) return true;
        }
    }

    if (t.screenSize || o.screenSize) {
        if (t.screenWidth != o.screenWidth) {
            if (!t.screenWidth) return false;
            if (!o.screenWidth) return true;
        }

        if (t.screenHeight != o.screenHeight) {
            if (!t.screenHeight) return false;
            if (!o.screenHeight) return true;
        }
    }

    if (t.version || o.version) {
        if (t.sdkVersion != o.sdkVersion) {
            if (!t.sdkVersion) return false;
            if (!o.sdkVersion) return true;
        }

        if (t.minorVersion != o.minorVersion) {
            if (!t.minorVersion) return false;
            if (!o.minorVersion) return true;
        }
    }
    return false;
}

bool isBetterThan(const ResTable_config& o,
        const ResTable_config* requested,
        const ResTable_config& t) {
    if (requested) {
        if (t.imsi || o.imsi) {
            if ((t.mcc != o.mcc) && requested->mcc) {
                return (t.mcc);
            }

            if ((t.mnc != o.mnc) && requested->mnc) {
                return (t.mnc);
            }
        }

        if (t.locale || o.locale) {
            if ((t.language[0] != o.language[0]) && requested->language[0]) {
                return (t.language[0]);
            }

            if ((t.country[0] != o.country[0]) && requested->country[0]) {
                return (t.country[0]);
            }
        }

        if (t.screenLayout || o.screenLayout) {
            if (((t.screenLayout^o.screenLayout) & MASK_LAYOUTDIR) != 0
                    && (requested->screenLayout & MASK_LAYOUTDIR)) {
                int myLayoutDir = t.screenLayout & MASK_LAYOUTDIR;
                int oLayoutDir = o.screenLayout & MASK_LAYOUTDIR;
                return (myLayoutDir > oLayoutDir);
            }
        }

        if (t.smallestScreenWidthDp || o.smallestScreenWidthDp) {
            // The configuration closest to the actual size is best.
            // We assume that larger configs have already been filtered
            // out at this point.  That means we just want the largest one.
            if (t.smallestScreenWidthDp != o.smallestScreenWidthDp) {
                return t.smallestScreenWidthDp > o.smallestScreenWidthDp;
            }
        }

        if (t.screenSizeDp || o.screenSizeDp) {
            // "Better" is based on the sum of the difference between both
            // width and height from the requested dimensions.  We are
            // assuming the invalid configs (with smaller dimens) have
            // already been filtered.  Note that if a particular dimension
            // is unspecified, we will end up with a large value (the
            // difference between 0 and the requested dimension), which is
            // good since we will prefer a config that has specified a
            // dimension value.
            int myDelta = 0, otherDelta = 0;
            if (requested->screenWidthDp) {
                myDelta += requested->screenWidthDp - t.screenWidthDp;
                otherDelta += requested->screenWidthDp - o.screenWidthDp;
            }
            if (requested->screenHeightDp) {
                myDelta += requested->screenHeightDp - t.screenHeightDp;
                otherDelta += requested->screenHeightDp - o.screenHeightDp;
            }
            //ALOGI("Comparing this %dx%d to other %dx%d in %dx%d: myDelta=%d otherDelta=%d",
            //    screenWidthDp, screenHeightDp, o.screenWidthDp, o.screenHeightDp,
            //    requested->screenWidthDp, requested->screenHeightDp, myDelta, otherDelta);
            if (myDelta != otherDelta) {
                return myDelta < otherDelta;
            }
        }

        if (t.screenLayout || o.screenLayout) {
            if (((t.screenLayout^o.screenLayout) & MASK_SCREENSIZE) != 0
                    && (requested->screenLayout & MASK_SCREENSIZE)) {
                // A little backwards compatibility here: undefined is
                // considered equivalent to normal.  But only if the
                // requested size is at least normal; otherwise, small
                // is better than the default.
                int mySL = (t.screenLayout & MASK_SCREENSIZE);
                int oSL = (o.screenLayout & MASK_SCREENSIZE);
                int fixedMySL = mySL;
                int fixedOSL = oSL;
                if ((requested->screenLayout & MASK_SCREENSIZE) >= SCREENSIZE_NORMAL) {
                    if (fixedMySL == 0) fixedMySL = SCREENSIZE_NORMAL;
                    if (fixedOSL == 0) fixedOSL = SCREENSIZE_NORMAL;
                }
                // For screen size, the best match is the one that is
                // closest to the requested screen size, but not over
                // (the not over part is dealt with in match() below).
                if (fixedMySL == fixedOSL) {
                    // If the two are the same, but 'this' is actually
                    // undefined, then the other is really a better match.
                    if (mySL == 0) return false;
                    return true;
                }
                if (fixedMySL != fixedOSL) {
                    return fixedMySL > fixedOSL;
                }
            }
            if (((t.screenLayout^o.screenLayout) & MASK_SCREENLONG) != 0
                    && (requested->screenLayout & MASK_SCREENLONG)) {
                return (t.screenLayout & MASK_SCREENLONG);
            }
        }

        if ((t.orientation != o.orientation) && requested->orientation) {
            return (t.orientation);
        }

        if (t.uiMode || o.uiMode) {
            if (((t.uiMode^o.uiMode) & MASK_UI_MODE_TYPE) != 0
                    && (requested->uiMode & MASK_UI_MODE_TYPE)) {
                return (t.uiMode & MASK_UI_MODE_TYPE);
            }
            if (((t.uiMode^o.uiMode) & MASK_UI_MODE_NIGHT) != 0
                    && (requested->uiMode & MASK_UI_MODE_NIGHT)) {
                return (t.uiMode & MASK_UI_MODE_NIGHT);
            }
        }

        if (t.screenType || o.screenType) {
            if (t.density != o.density) {
                // density is tough.  Any density is potentially useful
                // because the system will scale it.  Scaling down
                // is generally better than scaling up.
                // Default density counts as 160dpi (the system default)
                // TODO - remove 160 constants
                int h = (t.density?t.density:160);
                int l = (o.density?o.density:160);
                bool bImBigger = true;
                if (l > h) {
                    int t = h;
                    h = l;
                    l = t;
                    bImBigger = false;
                }

                int reqValue = (requested->density?requested->density:160);
                if (reqValue >= h) {
                    // requested value higher than both l and h, give h
                    return bImBigger;
                }
                if (l >= reqValue) {
                    // requested value lower than both l and h, give l
                    return !bImBigger;
                }
                // saying that scaling down is 2x better than up
                if (((2 * l) - reqValue) * h > reqValue * reqValue) {
                    return !bImBigger;
                } else {
                    return bImBigger;
                }
            }

            if ((t.touchscreen != o.touchscreen) && requested->touchscreen) {
                return (t.touchscreen);
            }
        }

        if (t.input || o.input) {
            const int keysHidden = t.inputFlags & MASK_KEYSHIDDEN;
            const int oKeysHidden = o.inputFlags & MASK_KEYSHIDDEN;
            if (keysHidden != oKeysHidden) {
                const int reqKeysHidden =
                        requested->inputFlags & MASK_KEYSHIDDEN;
                if (reqKeysHidden) {

                    if (!keysHidden) return false;
                    if (!oKeysHidden) return true;
                    // For compatibility, we count KEYSHIDDEN_NO as being
                    // the same as KEYSHIDDEN_SOFT.  Here we disambiguate
                    // these by making an exact match more specific.
                    if (reqKeysHidden == keysHidden) return true;
                    if (reqKeysHidden == oKeysHidden) return false;
                }
            }

            const int navHidden = t.inputFlags & MASK_NAVHIDDEN;
            const int oNavHidden = o.inputFlags & MASK_NAVHIDDEN;
            if (navHidden != oNavHidden) {
                const int reqNavHidden =
                        requested->inputFlags & MASK_NAVHIDDEN;
                if (reqNavHidden) {

                    if (!navHidden) return false;
                    if (!oNavHidden) return true;
                }
            }

            if ((t.keyboard != o.keyboard) && requested->keyboard) {
                return (t.keyboard);
            }

            if ((t.navigation != o.navigation) && requested->navigation) {
                return (t.navigation);
            }
        }

        if (t.screenSize || o.screenSize) {
            // "Better" is based on the sum of the difference between both
            // width and height from the requested dimensions.  We are
            // assuming the invalid configs (with smaller sizes) have
            // already been filtered.  Note that if a particular dimension
            // is unspecified, we will end up with a large value (the
            // difference between 0 and the requested dimension), which is
            // good since we will prefer a config that has specified a
            // size value.
            int myDelta = 0, otherDelta = 0;
            if (requested->screenWidth) {
                myDelta += requested->screenWidth - t.screenWidth;
                otherDelta += requested->screenWidth - o.screenWidth;
            }
            if (requested->screenHeight) {
                myDelta += requested->screenHeight - t.screenHeight;
                otherDelta += requested->screenHeight - o.screenHeight;
            }
            if (myDelta != otherDelta) {
                return myDelta < otherDelta;
            }
        }

        if (t.version || o.version) {
            if ((t.sdkVersion != o.sdkVersion) && requested->sdkVersion) {
                return (t.sdkVersion > o.sdkVersion);
            }

            if ((t.minorVersion != o.minorVersion) &&
                    requested->minorVersion) {
                return (t.minorVersion);
            }
        }

        return false;
    }
    return isMoreSpecificThan(o, t);
}

String8 toString(const ResTable_config& t) {
    String8 res;

    if (t.mcc != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dmcc", dtohs(t.mcc));
    }
    if (t.mnc != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dmnc", dtohs(t.mnc));
    }
    if (t.language[0] != 0) {
        if (res.size() > 0) res.append("-");
        res.append(t.language, 2);
    }
    if (t.country[0] != 0) {
        if (res.size() > 0) res.append("-");
        res.append(t.country, 2);
    }
    if ((t.screenLayout&MASK_LAYOUTDIR) != 0) {
        if (res.size() > 0) res.append("-");
        switch (t.screenLayout&MASK_LAYOUTDIR) {
            case LAYOUTDIR_LTR:
                res.append("ldltr");
                break;
            case LAYOUTDIR_RTL:
                res.append("ldrtl");
                break;
            default:
                res.appendFormat("layoutDir=%d",
                        dtohs(t.screenLayout&MASK_LAYOUTDIR));
                break;
        }
    }
    if (t.smallestScreenWidthDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("sw%ddp", dtohs(t.smallestScreenWidthDp));
    }
    if (t.screenWidthDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("w%ddp", dtohs(t.screenWidthDp));
    }
    if (t.screenHeightDp != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("h%ddp", dtohs(t.screenHeightDp));
    }
    if ((t.screenLayout&MASK_SCREENSIZE) != SCREENSIZE_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.screenLayout&MASK_SCREENSIZE) {
            case SCREENSIZE_SMALL:
                res.append("small");
                break;
            case SCREENSIZE_NORMAL:
                res.append("normal");
                break;
            case SCREENSIZE_LARGE:
                res.append("large");
                break;
            case SCREENSIZE_XLARGE:
                res.append("xlarge");
                break;
            default:
                res.appendFormat("screenLayoutSize=%d",
                        dtohs(t.screenLayout&MASK_SCREENSIZE));
                break;
        }
    }
    if ((t.screenLayout&MASK_SCREENLONG) != 0) {
        if (res.size() > 0) res.append("-");
        switch (t.screenLayout&MASK_SCREENLONG) {
            case SCREENLONG_NO:
                res.append("notlong");
                break;
            case SCREENLONG_YES:
                res.append("long");
                break;
            default:
                res.appendFormat("screenLayoutLong=%d",
                        dtohs(t.screenLayout&MASK_SCREENLONG));
                break;
        }
    }
    if (t.orientation != ORIENTATION_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.orientation) {
            case ORIENTATION_PORT:
                res.append("port");
                break;
            case ORIENTATION_LAND:
                res.append("land");
                break;
            case ORIENTATION_SQUARE:
                res.append("square");
                break;
            default:
                res.appendFormat("orientation=%d", dtohs(t.orientation));
                break;
        }
    }
    if ((t.uiMode&MASK_UI_MODE_TYPE) != UI_MODE_TYPE_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.uiMode&MASK_UI_MODE_TYPE) {
            case UI_MODE_TYPE_DESK:
                res.append("desk");
                break;
            case UI_MODE_TYPE_CAR:
                res.append("car");
                break;
            case UI_MODE_TYPE_TELEVISION:
                res.append("television");
                break;
            case UI_MODE_TYPE_APPLIANCE:
                res.append("appliance");
                break;
            default:
                res.appendFormat("uiModeType=%d",
                        dtohs(t.screenLayout&MASK_UI_MODE_TYPE));
                break;
        }
    }
    if ((t.uiMode&MASK_UI_MODE_NIGHT) != 0) {
        if (res.size() > 0) res.append("-");
        switch (t.uiMode&MASK_UI_MODE_NIGHT) {
            case UI_MODE_NIGHT_NO:
                res.append("notnight");
                break;
            case UI_MODE_NIGHT_YES:
                res.append("night");
                break;
            default:
                res.appendFormat("uiModeNight=%d",
                        dtohs(t.uiMode&MASK_UI_MODE_NIGHT));
                break;
        }
    }
    if (t.density != DENSITY_DEFAULT) {
        if (res.size() > 0) res.append("-");
        switch (t.density) {
            case DENSITY_LOW:
                res.append("ldpi");
                break;
            case DENSITY_MEDIUM:
                res.append("mdpi");
                break;
            case DENSITY_TV:
                res.append("tvdpi");
                break;
            case DENSITY_HIGH:
                res.append("hdpi");
                break;
            case DENSITY_XHIGH:
                res.append("xhdpi");
                break;
            case DENSITY_XXHIGH:
                res.append("xxhdpi");
                break;
            case DENSITY_NONE:
                res.append("nodpi");
                break;
            default:
                res.appendFormat("%ddpi", dtohs(t.density));
                break;
        }
    }
    if (t.touchscreen != TOUCHSCREEN_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.touchscreen) {
            case TOUCHSCREEN_NOTOUCH:
                res.append("notouch");
                break;
            case TOUCHSCREEN_FINGER:
                res.append("finger");
                break;
            case TOUCHSCREEN_STYLUS:
                res.append("stylus");
                break;
            default:
                res.appendFormat("touchscreen=%d", dtohs(t.touchscreen));
                break;
        }
    }
    if (t.keyboard != KEYBOARD_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.keyboard) {
            case KEYBOARD_NOKEYS:
                res.append("nokeys");
                break;
            case KEYBOARD_QWERTY:
                res.append("qwerty");
                break;
            case KEYBOARD_12KEY:
                res.append("12key");
                break;
            default:
                res.appendFormat("keyboard=%d", dtohs(t.keyboard));
                break;
        }
    }
    if ((t.inputFlags&MASK_KEYSHIDDEN) != 0) {
        if (res.size() > 0) res.append("-");
        switch (t.inputFlags&MASK_KEYSHIDDEN) {
            case KEYSHIDDEN_NO:
                res.append("keysexposed");
                break;
            case KEYSHIDDEN_YES:
                res.append("keyshidden");
                break;
            case KEYSHIDDEN_SOFT:
                res.append("keyssoft");
                break;
        }
    }
    if (t.navigation != NAVIGATION_ANY) {
        if (res.size() > 0) res.append("-");
        switch (t.navigation) {
            case NAVIGATION_NONAV:
                res.append("nonav");
                break;
            case NAVIGATION_DPAD:
                res.append("dpad");
                break;
            case NAVIGATION_TRACKBALL:
                res.append("trackball");
                break;
            case NAVIGATION_WHEEL:
                res.append("wheel");
                break;
            default:
                res.appendFormat("navigation=%d", dtohs(t.navigation));
                break;
        }
    }
    if ((t.inputFlags&MASK_NAVHIDDEN) != 0) {
        if (res.size() > 0) res.append("-");
        switch (t.inputFlags&MASK_NAVHIDDEN) {
            case NAVHIDDEN_NO:
                res.append("navsexposed");
                break;
            case NAVHIDDEN_YES:
                res.append("navhidden");
                break;
            default:
                res.appendFormat("inputFlagsNavHidden=%d",
                        dtohs(t.inputFlags&MASK_NAVHIDDEN));
                break;
        }
    }
    if (t.screenSize != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("%dx%d", dtohs(t.screenWidth), dtohs(t.screenHeight));
    }
    if (t.version != 0) {
        if (res.size() > 0) res.append("-");
        res.appendFormat("v%d", dtohs(t.sdkVersion));
        if (t.minorVersion != 0) {
            res.appendFormat(".%d", dtohs(t.minorVersion));
        }
    }

    return res;
}
