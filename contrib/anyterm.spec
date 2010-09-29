%define name anyterm
%define version 1.1.1
%define vendor urs
%define vepoch 3
%define rotedir /usr/lib/rote-0.2.7/include

%define apxs /usr/sbin/apxs
%define moduledir %(apxs -q LIBEXECDIR)
%define wwwdir %{_localstatedir}/www
%define wwwetc %{_sysconfdir}/httpd/conf.d

Summary: Anyterm is a web-based terminal emulator.
Name: %{name}
Version: %{version}
Release: 0.%{vendor}.%{vepoch}%{?distro:_%distro}
License: GPL
Group: Applications/Internet
URL: http://anyterm.org
Source0: http://anyterm.org/download/anyterm-1.1.1.tbz2
Source100: anyterm.conf
Patch0: anyterm-setsid.patch
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

Requires: httpd
Requires: rote
BuildRequires: httpd-devel
BuildRequires: rote-devel

%description
This is the Anyterm web-based terminal emulator.

%prep
%setup -q
%patch -p1

%build

cd apachemod
%{__make} \
  APXS2="%{apxs}" \
  ROTE_PRIVATE_DIR="%{rotedir}"
cd ..

cd anygetty
%{__make} \
  CFLAGS="$RPM_OPT_FLAGS"
cd ..

/bin/cat > anyterm.conf <<"END"
%include %{SOURCE100}
END

%install
rm -rf $RPM_BUILD_ROOT

%{__mkdir_p} $RPM_BUILD_ROOT%{moduledir}

cd apachemod
%makeinstall \
  APXS2="%{apxs} -S LIBEXECDIR=$RPM_BUILD_ROOT%{moduledir}"
cd ..

%{__mkdir_p} $RPM_BUILD_ROOT%{_bindir}
%{__cp} anygetty/anygetty $RPM_BUILD_ROOT%{_bindir}/anygetty

%{__mkdir_p} $RPM_BUILD_ROOT%{wwwdir}/anyterm
cd browser
/usr/bin/find . \
  ! -name "*~" \
  -print \
| /bin/cpio --quiet -pmd $RPM_BUILD_ROOT%{wwwdir}/anyterm
cd ..

%{__mkdir_p} $RPM_BUILD_ROOT%{wwwetc}
%{__cp} anyterm.conf $RPM_BUILD_ROOT%{wwwetc}/anyterm.conf

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc CHANGELOG LICENSE README
%{moduledir}/anyterm.so
%attr(04550,root,apache) %{_bindir}/anygetty
%{wwwdir}/anyterm
%config(noreplace) %{wwwetc}/anyterm.conf
%config(noreplace) %{wwwdir}/anyterm/.htaccess

%changelog
* Sun Jun 12 2005 Carl D. Roth <roth@ursus.net> - 
- Initial build.

