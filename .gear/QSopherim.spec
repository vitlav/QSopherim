Name: QSopherim
Version: 0.1
Release: alt0.3

Summary: QSopherim
Packager: Vitaly Lipatov <lav@altlinux.ru>

Url: https://github.com/WarmongeR1/QSopherim
License: GPLv3
Group: Text tools

# https://github.com/WarmongeR1/QSopherim
Source: %name-%version.tar

# Automatically added by buildreq on Tue Feb 12 2013
# optimized out: fontconfig libqt4-core libqt4-devel libqt4-gui libqt4-xml libstdc++-devel
BuildRequires: gcc-c++ glibc-devel phonon-devel

%description
QSopherim.

%prep
%setup

%build
export QTDIR=%_qt4dir/
export PATH=$PATH:$QTDIR/bin

%define qmake_qt4 qmake-qt4
%qmake_qt4 %name.pro
%make_build

%install
%makeinstall INSTALL_ROOT=%buildroot

install -m 755 -D build/bin/%name %buildroot%_bindir/%name
install -m 644 -D desktop/%name.desktop %buildroot%_desktopdir/%name.desktop
install -m 644 -D desktop/%{name}_logo.png %buildroot%_iconsdir/hicolor/128x128/apps/%name.png
install -d %buildroot%_datadir/%name/
cp -a data/* %buildroot%_datadir/%name/

%files
%_bindir/%name
%_datadir/%name/
%_desktopdir/%name.desktop
%_iconsdir/hicolor/128x128/apps/%name.png
#%_iconsdir/hicolor/scalable/apps/%name.svg

%changelog
* Wed Mar 20 2013 Vitaly Lipatov <lav@altlinux.ru> 0.1-alt0.3
- pack data, desktop, icon

* Mon Mar 11 2013 Vitaly Lipatov <lav@altlinux.ru> 0.1-alt0.2
- new build

* Tue Feb 12 2013 Vitaly Lipatov <lav@altlinux.ru> 0.1-alt0.1
- initial build for ALT Linux Sisyphus
