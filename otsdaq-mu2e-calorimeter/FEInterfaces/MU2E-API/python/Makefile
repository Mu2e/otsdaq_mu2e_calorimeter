# Makefile

# List here Designer files
# UIFILES = MainWindow.ui
UIFILES = $(wildcard *.ui)
# QRCFILES = resources.qrc

# Python files that will be created
PYTHONTARGETS = $(UIFILES:.ui=.py) #$(QRCFILES:.qrc=_rc.py) 

.PHONY: all clean

# Target: all (default target)
all: ui_resources

# Target: ui_resources
ui_resources: $(PYTHONTARGETS)

# Pattern rule for generating .py from .ui
%.py: %.ui
	pyuic5 -o $@ $<

# Pattern rule for generating _rc.py from .qrc
%_rc.py: %.qrc
	pyrcc5 -o $@ $<

# Target: clean
clean:
	rm -f $(PYTHONTARGETS)