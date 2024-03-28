python .\phanapi\scripts\make_headers.py .\definitions\api.xml include\AGPU
python .\phanapi\scripts\make_headers_cpp.py .\definitions\api.xml include\AGPU
python .\phanapi\scripts\make_implementation_stubs_cpp.py .\definitions\api.xml include\AGPU
python .\phanapi\scripts\make_icdloader.py .\definitions\api.xml implementations\Loader
python .\phanapi\scripts\make_pharo_bindings.py .\definitions\api.xml tonel
python .\phanapi\scripts\make_pharo_bindings.py -squeak .\definitions\api.xml tonel
python .\phanapi\scripts\make_sysmel_bindings.py .\definitions\api.xml bindings\sysmel\module-sources\Bindings.AbstractGPU\AbstractGPU-Bindings\bindings.sysmel
