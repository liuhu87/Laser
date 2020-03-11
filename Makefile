NAME := temp.exe
TARGET :=main_temp

INCDIR := include
OBJDIR := obj

ARCH :=`root-config --arch`
UID  :=$(shell whoami)

HEADERS  := 
SOURCES  :=  $(TARGET).C
OBJS     :=  $(OBJDIR)/main.o

DEFINES  := -I. -I$(INCDIR) -I$(OBJDIR) -I`root-config --cflags`
ifeq ($(UID),hliu)
DEFINES  += -I/afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/include
endif
ifeq ($(UID),lix)
DEFINES  += -I/scratchfs/ybj/lix/WFCTA/include
endif

#CXXFLAGS += -D__DST_HE_12__
CXXFLAGS := -O3 -fPIC -Wno-write-strings
#CXXFLAGS += -D_THIN_
ifeq ($(UID),hliu)
CXXFLAGS += -D_hliu
endif
ifeq ($(UID),lix)
CXXFLAGS += -D_lix
endif

LDFLAGS  := `root-config --libs` 
#LDFLAGS  += -lCore -lMinuit -lNetx -lProof -lProofPlayer -lTMVA -lXMLIO -lMLP -lTreePlayer -lRFIO -lXrdClient -lGpad -lNet -lHist -lHistPainter -lGraf -lMatrix
ifeq ($(UID),hliu)
LDFLAGS  +=-L. /afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent/lib.so
#LDFLAGS  +=-L. /afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/lib/lib.so
endif
ifeq ($(UID),lix)
LDFLAGS  +=-L. /scratchfs/ybj/lix/Laser/lib.so
#LDFLAGS  +=-L. /scratchfs/ybj/lix/WFCTA/lib/lib.so
endif

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 

$(OBJDIR)/main.o: $(TARGET).C
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

$(OBJDIR)/%.o: $(OBJDIR)/%.C
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

clean:
	rm -fv $(OBJDIR)/* $(NAME)

