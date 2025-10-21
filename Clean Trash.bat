del *.obj *.ilk *.pdb *.tlb *.tli *.tlh *.tmp *.rsp *.pch *.idb *.sbr *.map *.bsc *.exp *.htm *.dep *.res *.svn *.exe.*.manifest *.dll.*.manifest *.tlog *.log *.ipch *.lastbuildstate unsuccessfulbuild *.unsuccessfulbuild *.ncb *.sdf /s
FOR /F delims^= %%A IN ('DIR/AD/B/S^|SORT/R') DO RD "%%A"
pause