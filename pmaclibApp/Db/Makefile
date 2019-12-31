TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Find any .sym files in the Db directory
vpath %.sym ..
vpath %-edb.def ..
vpath %-tdct.cfg ..

# Install database files created from .sch files
# using TDCT (or just install .db files) into <top>/db.
# TDCT_CFG can be redefined here if needed
#TDCT_CFG = ../tdct.cfg

# Make depends on all .sch files
DEPSCHS = $(wildcard ../*.sch)

# Add target .db files below here. Each must
# correspond to a .sch file

# If .sym files are in this support module,
# simply install them into <top>/db.
# Also install the additional entries for the system edb.def file
# and the TDCT configuration file into <top>/db.
#DB += $(wildcard ../*.sym)
DB += eloads.sym
DB += $(wildcard ../*-edb.def)
DB += $(wildcard ../*-tdct.cfg)

TEMPLATES_DIR=schematics
TEMPLATES += pmacUpDown.sch pmacUpDown.sym
TEMPLATES += pmacProgStatus.sch pmacProgStatus.sym


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
$(COMMON_DIR)/%.db: %.db
	        cp $< $@


clean:
	$(RM) *.dat *.bak
