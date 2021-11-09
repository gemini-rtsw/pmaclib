%define _prefix /gem_base/epics/support
%define name pmaclib
%define repository gemdev
%define debug_package %{nil}
%define arch %(uname -m)
%define checkout %(git log --pretty=format:'%h' -n 1) 

#These global defines are added to prevent stripping
# symbols on vxWorks cross-compiled code
# Getting 'strip' to work is probably only needed for
# building a related debug sub-package
#
# But this prevents all the strip warnings
# mrippa 20120202
%global _enable_debug_package 0
%global debug_package %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress %{nil}

Summary: %{name} Package, a module for EPICS base
Name: %{name}
Version: 1.1.12
Release: 2%{?dist}
License: EPICS Open License
Group: Applications/Engineering
Source0: %{name}-%{version}.tar.gz
ExclusiveArch: %{arch}
Prefix: %{_prefix}
## You may specify dependencies here
BuildRequires: epics-base-devel re2c gemini-ade gemUtil-devel geminiRec-devel psmisc
Requires: epics-base gemUtil geminiRec
## Switch dependency checking off
# AutoReqProv: no

%description
This is the module %{name}.

## If you want to have a devel-package to be generated uncomment the following:
%package devel
Summary: %{name}-devel Package
Group: Development/Gemini
Requires: %{name}
%description devel
This is the module %{name}.

%prep
%setup -q 

%build
# virtual X env for tdct
Xvfb :1  -ac -nolisten tcp -nolisten unix &

make distclean uninstall
DISPLAY=:1 make

killall Xvfb

%install
export DONT_STRIP=1
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r dbd $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r lib $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r include $RPM_BUILD_ROOT/%{_prefix}/%{name}
cp -r configure $RPM_BUILD_ROOT/%{_prefix}/%{name}


%postun
if [ "$1" = "0" ]; then
    rm -rf %{_prefix}/%{name}
fi


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
   /%{_prefix}/%{name}/lib

%files devel
%defattr(-,root,root)
   /%{_prefix}/%{name}/dbd
   /%{_prefix}/%{name}/include
   /%{_prefix}/%{name}/configure

%changelog
* Thu Oct 08 2020 fkraemer <fkraemer@gemini.edu> 1.1.12-2
- applied new version/release scheme and new yum repository structure

* Fri Aug 28 2020 Felix Kraemer <fkraemer@gemini.edu> 3.15.8-1.1.12.2020082821234d2d4e7
- adjustments to read in local configuration configure/RELEASE.local from
  within configure/RELEASE (fkraemer@gemini.edu)
- Release tag enriched with hour and minute (%%H%%M) to be able to build
  several RPMs a day without messing up the repo (fkraemer@gemini.edu)

* Sun Jul 26 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-1.1.12.202007262e0544f
- new package built with tito

* Sun Jul 26 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-2.1.12.202007260e35513
- new package built with tito

* Sun Jul 26 2020 fkraemer <fkraemer@gemini.edu> 3.15.8-2.1.12.20200726fa2e3ca
- new package built with tito

