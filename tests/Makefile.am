if HAVE_TESTS
TESTS = unit_t

inst_checkdir=$(docdir)/tests/
inst_check_PROGRAMS= unit_t
unit_t_CFLAGS =\
			   -I$(top_srcdir)/packaged/include\
			   -I$(top_srcdir)/packaged/src\
			   $(ZMQ_CFLAGS)\
			   $(UUID_CFLAGS)\
			   @TST_CPPFLAGS@

unit_t_LDFLAGS =\
				@TST_LDFLAGS@\
				$(ZMQ_LIBS)\
				$(UUID_LIBS)

unit_t_LDADD =\
			  $(top_builddir)/packaged/lib/libbxiutil.la\
			  @TST_LIBS@

#TESTS_ENVIRONMENT=@VALGRIND@ @VALGRIND_ARGS@
AUTOMAKE_OPTIONS = parallel-tests
TEST_EXTENSIONS = .pl .sh
LOG_COMPILER =${VALGRIND}  `if   test "${VALGRIND}" !=  ""   ; then echo "${VALGRIND_ARGS}"; fi`
LOG_DRIVER=$(top_srcdir)/custom-test-driver

compile_tests:$(check_PROGRAMS)

unit_t$(EEXT):force

force:${top_srcdir}/valgrind.supp
	mkdir -p report
	-[ -f $$(basename $^)  ] || cp $^ .

unit_t_SOURCES=\
			   unit_t.c

EXTRA_DIST=\
		   test_kvl.c\
		   test_map.c\
		   test_misc.c\
		   test_rng.c\
		   test_stretch.c\
		   test_vector.c

DISTCLEANFILES=\
			   valgrind.supp\
			   ${PACKAGE_NAME}.bxilog\
			   unit_t.bxilog\
			   report/${PACKAGE_NAME}-Results.xml\
			   report/${PACKAGE_NAME}-Listing.xml
endif
