AC_PREREQ(2.53)

AC_INIT(Iris, 0.1, xxx@users.sourceforge.net, Iris)
AC_CONFIG_SRCDIR(autogen.sh)

AM_INIT_AUTOMAKE([1.8 gnu dist-zip])

##### Alternatives

# Generate documentation 
# Note: AutoMake conditional will be set when 'doxygen' is checked for.
#
AC_ARG_ENABLE(
	doc,
	AC_HELP_STRING(
		[--enable-doc],
		[Generate documentation (default=no)]),
	[ case "$enableval" in
		yes)	enable_doc=yes ;;
		no)	enable_doc=no  ;;
		*)	AC_MSG_ERROR([bad value '$enableval' for --enable-doc]) ;;
	  esac
	],
	[enable_doc=no]
)
AM_CONDITIONAL(DOC, test "$enable_doc" = "yes")
AC_MSG_CHECKING(whether to generate documentation)
AC_MSG_RESULT($enable_doc)

if test "$enable_doc" = "yes"; then
	AC_CHECK_PROG(x_doxygen, doxygen, yes, no)
	if test "$x_doxygen" = "no"; then
		AC_MSG_WARN(['doxygen' not found. You will not be able to generate code documentation])
		enable_doc=no
	fi
fi


##### Output

AC_SUBST(prefix)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

