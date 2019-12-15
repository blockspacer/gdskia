#pragma once

#include "core/reference.h"
#include "core/resource.h"
#include "scene/2d/mesh_instance_2d.h"
#include "scene/resources/texture.h"

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkString.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkGraphics.h>
#include <skia/include/core/SkPictureRecorder.h>
#include <skia/include/core/SkStream.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkShader.h>
#include "skia/include/core/SkPath.h"
#include <skia/modules/skottie/include/Skottie.h>
#include <skia/modules/skottie/utils/SkottieUtils.h>
#include <skia/src/core/SkMakeUnique.h>
#include <skia/include/effects/SkGradientShader.h>
#include <skia/src/core/SkOSFile.h>
#include <skia/src/utils/SkOSPath.h>
#include <skia/include/core/SkMaskFilter.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkTypeface.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkStream.h>
#include <skia/include/core/SkDocument.h>
#include "skia/include/core/SkRefCnt.h"
#include "skia/include/core/SkTime.h"
#include "skia/include/core/SkImage.h"


class GdSkia : /*public Node2D*/public Reference {
    GDCLASS(GdSkia, Reference);

    int count;

protected:
    static void _bind_methods();

public:
    GdSkia();

public:
    void set_image_size(int p_width, int p_height);
    //Ref<Image> get_image() /*const*/; // TODO: make const
    Ref<ImageTexture> get_texture() /*const*/; // TODO: make const
		//Ref<ImageTexture> get_image(); // TODO: make const
		void update_image_if_needed();

private:
	int width_ = 1024;
	int height_ = 1024;

	// see https://docs.godotengine.org/en/3.0/classes/class_imagetexture.html

  // see https://docs.godotengine.org/en/3.0/classes/class_image.html
	//Image* img_ = nullptr;
	Ref<Image> img_;
	Ref<ImageTexture> tex_;
};

