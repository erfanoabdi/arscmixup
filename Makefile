#author AngelToms
CXXFLAGS	:=
CFLAGS		:=
CLIBS		:=
CINCLUDES	:=  -I./dto -I./fcreator -I./parser -I./res -I./source -I./utils -I./dto/simple -I./mixer -I./zlib -I./zip -I./zipapk

all	: arscmaker

objs	:=  res/ResTableConfig.o		\
	    utils/Debug.o			\
	    utils/Unicode.o			\
	    utils/SharedBuffer.o		\
	    utils/Static.o			\
	    utils/String8.o			\
	    utils/String16.o			\
	    utils/FileScan.o			\
	    dto/ResTab.o			\
	    dto/ResStringPool.o			\
	    dto/ResTabHeader.o			\
	    dto/simple/SimResTab.o		\
	    dto/simple/SimResStringPool.o	\
	    dto/simple/SimResPackage.o		\
	    dto/simple/SimResTypeSpec.o		\
	    dto/simple/SimResType.o		\
	    dto/simple/SimResTypes.o		\
	    parser/Parser.o			\
	    parser/ArscComplexParser.o		\
	    parser/ArscSimpleParser.o		\
	    fcreator/FileNodeOp.o		\
	    fcreator/FCreator.o			\
	    mixer/Mixer.o			\
	    mixer/ArscMixer.o			\
	    zlib/zipfile.o			\
	    zlib/zutil.o			\
	    zlib/adler32.o			\
	    zlib/compress.o			\
	    zlib/crc32.o			\
	    zlib/deflate.o			\
	    zlib/gzio.o				\
	    zlib/infback.o			\
	    zlib/inffast.o			\
	    zlib/inflate.o			\
	    zlib/inftrees.o			\
	    zlib/trees.o			\
	    zlib/uncompr.o			\
	    zlib/centraldir.o			\
	    zip/ZipEntry.o			\
	    zip/ZipFile.o			\
	    zip/ZipUtils.o			\
	    zip/ZipFileRO.o			\
	    zip/FileMap.o			\
	    zip/misc.o				\
	    zip/VectorImpl.o			\
	    zipapk/ApkZipOp.o			\
	    main.o
	

arscmaker: $(objs)

	g++ $(objs) $(CLIBS) $(CINCLUDES) -o arscparser 

clean:
	rm -rf *.o arscmaker arscmaker.exe utils/*.o fcreator/*.o parser/*.o source/*.o dto/*.o dto/simple/*.o mixer/*.o zlib/*.o zip/*.o zipakp/*.o
		

		
	