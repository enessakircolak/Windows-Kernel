@echo off

sc stop esc
sc delete esc
sc create esc type=kernel binpath=C:\Users\scrt\Desktop\PrivESC.sys
sc start esc