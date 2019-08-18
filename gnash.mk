#############################################################
#
# gnash
#
#############################################################
GNASH_VERSION:=0.8.11
GNASH_SITE=$(TOPDIR)/package/gnash
GNASH_SITE_METHOD=local
GNASH_BUILD_DIR = $(BUILD_DIR)
GNASH_INSTALL_STAGING = YES
GNASH_INSTALL_TARGET = YES
#GNASH_DEPENDENCIES = zlib alsa-lib libcurl
GNASH_DEPENDENCIES = libcurl

export GNASH_STAGING_DIR = $(STAGING_DIR)
export GNASH_TARGET_DIR = $(TARGET_DIR)

#export PATH=$(HOST_DIR)/usr/bin

export CROSS=$(HOST_DIR)/usr/bin/$(BR2_ARCH)-linux-gnu-

define GNASH_CONFIGURE_CMDS
		cd $(@D)/external/agg-2.5; \
		make -j4; \
		make clean; \
		cd ../..; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--prefix=$(TARGET_DIR)/usr \
		--host=$(BR2_ARCH)-linux-gnu \
		\
		 --enable-ssl \
		\
		--with-freetype-incl=$(STAGING_DIR)/usr/include/freetype2 \
		--with-freetype-lib=$(STAGING_DIR)/usr/lib \
		--with-curl-incl=$(STAGING_DIR)/usr/include \
		--with-curl-lib=$(STAGING_DIR)/usr/lib \
		--with-sdl-incl=$(STAGING_DIR)/usr/include/SDL \
		--with-sdl-lib=$(STAGING_DIR)/usr/lib \
		--with-boost-incl=$(STAGING_DIR)/usr/include/boost \
		--with-boost-lib=$(STAGING_DIR)/usr/lib \
		\
		--enable-extensions=fileio,dbus,gtk2 \
		\
		--with-sysroot=$(STAGING_DIR) \
		\
		--disable-rpath \
		--enable-docbook \
		--enable-log \
		--enable-media=ffmpeg \
		--disable-npapi \
		--enable-renderer=agg \
		--with-agg-incl=$(@D)/external/agg-2.5/include \
		--with-agg-lib=$(@D)/external/agg-2.5/lib \
		--enable-gui=fb \
		--enable-sound=sdl \
		--enable-device=rawfb \
		--disable-jemalloc
endef

#		--disable-rpath \
#		--enable-cygnal \
#		--with-plugins-install=prefix \

#		--with-sysroot=$(STAGING_DIR) \

#		--with-boost-incl=$(STAGING_DIR)/usr/include/boost \
#		--with-boost-lib=$(STAGING_DIR)/usr/lib \


#		--extra-ldflags='-L$(STAGING_DIR)/usr/lib/ -L$(STAGING_DIR)/usr/lib/libplayer -lamavutils -ldl' \

define GNASH_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) ARCH=$(BR2_ARCH) $(MAKE) -C $(@D) all
	$(TARGET_CONFIGURE_OPTS) ARCH=$(BR2_ARCH) $(MAKE) -C $(@D) install
endef

define GNASH_CLEAN_CMDS
	$(MAKE) -C $(@D) clean
endef

$(eval $(generic-package))
