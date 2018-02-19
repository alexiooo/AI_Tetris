export LC_ALL=C
cat tmp/* | sort -t',' -n |
sed -E -e '/^(-?[0-9]+),(-?[0-9]+)/{
	   s/^(-?[0-9]+(,-?[0-9]+){3}),(-?[0-9]+)/\1,=\3/
       $!{          # append the next line when not on the last line
	     :loop
		 N
		 
         s/^(-?[0-9]+(,-?[0-9]+){3}),(=[0-9]+(\+[0-9]+)*)\s+\1,([0-9]+)/\1,\3\+\5/
                    # now test for a successful substitution, otherwise
                    #+  unpaired "a test" lines would be mis-handled
         t loop  # branch_on_substitute (goto label :sub-yes)
         :sub-not   # a label (not essential; here to self document)
                    # if no substituion, print only the first line
         P          # pattern_first_line_print
         D          # pattern_ltrunc(line+nl)_top/cycle
       }    
     }'