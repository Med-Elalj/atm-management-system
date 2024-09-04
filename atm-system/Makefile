# List of object files
objects = src/main.o src/system.o src/auth.o src/db.o

# The default target
atm: $(objects)
	rm -f atm
	cc -o atm $(objects) -lsqlite3
	rm -f $(objects)


# Rule to clean up the build artifacts
clean:
	rm -f $(objects)
