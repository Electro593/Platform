CC := gcc
LD := ld

SRCS := $(shell find $(SRC_DIR) -name '*.c')
DEPS := $(patsubst $(SRC_DIR)/%.c, $(TMP_DIR)/%.d, $(SRCS))
OBJS := $(DEPS:.d=.o)

PCH := $(shell find $(SRC_DIR) -maxdepth 1 -type f -name 'pch.h')
GCH := $(patsubst $(SRC_DIR)/%.h, $(TMP_DIR)/%.h.gch, $(PCH))
GCHD := $(GCH:.h.gch=.h.gch.d)
GCHI := $(GCH:.h.gch=.h.gch.i)

.SECONDARY:

$(GCH): $(PCH)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -x c-header -MMD -MP -MT $@ -MF $(GCHD) -o $@ $<

$(GCHI): $(PCH)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -x c-header -E -o $@ $<

$(TMP_DIR)/%.o: $(SRC_DIR)/%.c | $(GCH)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(if $(PCH),-include $(abspath $(basename $(GCH))),) -MMD -MP -MT $@ -MF $(@:.o=.d) -save-temps=obj -o $@ $<

$(OUT_DIR)/%.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	readelf -a $@ > $@.dump

$(OUT_DIR)/%.dll: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	objdump -x $@ > $@.dump

-include $(DEPS) $(GCHD)
