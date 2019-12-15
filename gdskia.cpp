#include "gdskia.h"

#include "core/error_macros.h"
#include "core/io/marshalls.h"
#include "core/os/os.h"
#include "core/print_string.h"
#include "core/project_settings.h"
#include "core/ustring.h"

static sk_sp<SkSurface> sRasterSurface;
//static SkPixmap browserPixmapCache;
//static sk_sp<SkImage> skImageSp;
static sk_sp<SkImage> UIDemoImage;

#define DEBUG_PRINT(m_p) print_line(m_p)
//#define DEBUG_PRINT(m_p)

/*Ref<Image> GdSkia::get_image() const {
		const int SIZE = width_ * height_;
		//Image::Format image_format = Image::Format::FORMAT_RGBA8; // FORMAT_TEXTURE FORMAT_RGBA8
    Ref<Image> resImg = memnew(Image);
		PoolVector<uint8_t> dest_data;
		dest_data.resize(SIZE);
		PoolVector<uint8_t>::Write w = dest_data.write();
		for (int i = 0; i < SIZE; ++i) {
			w[i] = (uint8_t)(0.5 * 255.0);
		}
		return resImg;
}*/

void GdSkia::update_image_if_needed() {
	if (img_.is_null()) {
		img_ = Ref<Image>(memnew(Image(width_, height_, true, Image::Format::FORMAT_RGBA8)));
	}

	if(!sRasterSurface || sRasterSurface->width() != width_ || sRasterSurface->height() != height_) {
		// re-create the surface, see https://groups.google.com/forum/#!topic/skia-discuss/3c10MvyaSug
		const SkImageInfo info = SkImageInfo::MakeN32(
				width_, height_,
				kPremul_SkAlphaType); // TODO
		sRasterSurface = SkSurface::MakeRaster(info); // TODO: mutex & threading
		if (!sRasterSurface) {
			printf("failed to create raster surface\n");
		}
	}

	ERR_FAIL_COND(!sRasterSurface);
	SkCanvas *uicanvas =
			sRasterSurface->getCanvas();
	ERR_FAIL_COND(!uicanvas);

	uicanvas->clear(SkColorSetARGB(255, 0, 255, 0));
	sRasterSurface->flush();
	{
		UIDemoImage = sRasterSurface->makeImageSnapshot();
		if (nullptr == UIDemoImage) {
			printf("UIDemoImage can`t makeImageSnapshot\n");
		}
	}
	SkPixmap pixmap;
	if (!UIDemoImage->peekPixels(&pixmap)) {
		///if (isDebugPeriodReached())
		printf("can`t peekPixels\n");
	}
	SkIRect pixmap_clip = pixmap.bounds();
	ERR_FAIL_COND(pixmap_clip.width() != width_);
	ERR_FAIL_COND(pixmap_clip.height() != height_);

	ERR_FAIL_COND(img_.is_null());

	if (img_->get_width() != width_ || img_->get_height() != height_) {
		img_->crop(width_, height_);
	}

	img_->lock();
	//const uint32_t* data = static_cast<const uint32_t*>(pixmap.addr());
	for (int j = 0; j < height_; j++) {
		for (int i = 0; i < width_; i++) {
			//if(data) {
			//const uint32_t pixel = *(data++);
			SkColor pixel = pixmap.getColor(i, j);
			///Color rgb = Color(0, 1, 0, 1);

			// TODO (!!!): speed up by SkConvertPixels & mem copy, see Godot::Image::load_png_from_buffer (!!!)

			Color rgb = Color(SkColorGetR(pixel) / 255.0,
					SkColorGetG(pixel) / 255.0,
					SkColorGetB(pixel) / 255.0,
					SkColorGetA(pixel) / 255.0);

			img_->set_pixel(i, j, rgb);
			//}
		}
	}
	img_->unlock();

	ERR_FAIL_COND(img_->empty() || img_->get_data().size() < 0);
	ERR_FAIL_COND(img_->get_width() != width_);
	ERR_FAIL_COND(img_->get_height() != height_);
}

/*Ref<Image> GdSkia::get_image() {
	//DEBUG_PRINT("get_image size " + itos(width_) + " " + itos(height_) + "\n");

	update_image_if_needed();

	ERR_FAIL_COND_V(img_.is_null(), nullptr);
	return img_;
}
*/

Ref<ImageTexture> GdSkia::get_texture() {
	//DEBUG_PRINT("get_texture size " + itos(width_) + " " + itos(height_) + "\n");

	update_image_if_needed();

	ERR_FAIL_COND_V(img_.is_null(), nullptr);

	if (tex_.is_null()) {
		tex_ = Ref<ImageTexture>(memnew(ImageTexture()));
		img_->lock();
		tex_->create_from_image(img_, ImageTexture::Flags::FLAG_FILTER | ImageTexture::Flags::FLAG_MIPMAPS);
		img_->unlock();
	} else {
		img_->lock();
		tex_->create_from_image(img_, ImageTexture::Flags::FLAG_FILTER | ImageTexture::Flags::FLAG_MIPMAPS);
		//tex_->set_data(img_);
		img_->unlock();
	}

	ERR_FAIL_COND_V(tex_.is_null(), nullptr);
	ERR_FAIL_COND_V(tex_->get_width() < 0 || tex_->get_height() < 0, nullptr);
	ERR_FAIL_COND_V(tex_->get_size() != img_->get_size(), nullptr);
	ERR_FAIL_COND_V(tex_->get_width() != width_, nullptr);
	ERR_FAIL_COND_V(tex_->get_height() != height_, nullptr);

	return tex_;
}

/*Ref<ImageTexture> GdSkia::get_image()
{
	Image* img = memnew(Image(width_, height_, true, Image::Format::FORMAT_RGBA8));
	ImageTexture* tex = memnew(ImageTexture());

	img->lock();
	for (int j = 0; j < height_; j++)
	{
		for (int i = 0; i < width_; i++)
		{
			Color rgb = Color(0, 1, 0, 1);
			img->set_pixel(i, j, rgb);
		}
	}
	img->unlock();

	tex->create_from_image(img, ImageTexture::Flags::FLAG_ANISOTROPIC_FILTER | ImageTexture::Flags::FLAG_MIPMAPS);

	return tex;
}*/

void GdSkia::set_image_size(int p_width, int p_height) {
	ERR_FAIL_COND(p_width < 0);
	ERR_FAIL_COND(p_height < 0);

	width_ = p_width;
	height_ = p_height;

	get_texture(); // updates texture

	ERR_FAIL_COND(tex_.is_null());
	DEBUG_PRINT("set_image_size prev tex_ size" + itos(tex_->get_width()) + " " + itos(tex_->get_height()) + "\n");

	DEBUG_PRINT("set_image_size new size " + itos(width_) + " " + itos(height_) + "\n");
}

void GdSkia::_bind_methods() {
	//ClassDB::bind_method(D_METHOD("get_image"), &GdSkia::get_image);
	ClassDB::bind_method(D_METHOD("get_texture"), &GdSkia::get_texture);
	ClassDB::bind_method(D_METHOD("set_image_size", "width", "height"), &GdSkia::set_image_size);
}

GdSkia::GdSkia() {
}

// TODO: free mem
/*GdSkia::~GdSkia() {
  if (sRasterSurface.get())
    delete sRasterSurface.release();
}*/
