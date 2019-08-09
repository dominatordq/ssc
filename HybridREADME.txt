Before running sdktool, check:

-Make sure the folder 'pv_data' is copied and pasted into the location of your .lk script file.
-Line 2513 and 2521 of csp_dispatch.cpp - change output file destination to your custom location.
-If you'd like to print out all parameters to text files, call the O.output_parameters(csp_dispatch_opt *optinst, unordered_map<std::string, double> &pars, int nt) function found on line 3049.
 Just change the location of where the files output.