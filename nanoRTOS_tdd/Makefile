#####################
# Build All Targets #
#####################

# include all the subdir to include all the makefiles
SUBDIRS +=      Testers/Main
SUBDIRS +=      

# Add for clean all
SUBDIRSCLEAN=$(addsuffix clean,$(SUBDIRS))

.PHONY: $(SUBDIRS)
# All the Targets
all: $(SUBDIRS)

# Invoke all the subdir makefiles
$(SUBDIRS):
	$(MAKE) -C $@

# Clean all
clean: $(SUBDIRSCLEAN)

clean_curdir:
	rm -f build/cut/*.o build/cut/*.d build/cut/*.a build/*.o build/*.d build/*.exe

%clean: %
	$(MAKE) -C $< -f $(PWD)/Makefile clean_curdir

############################
# Build Individual Targets #
############################
# main Target
Main: main
# Invoke the makefile in the "main" subdir
main:
	$(MAKE) -C Testers/Main

# EOF
