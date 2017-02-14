(function($){
    $.fn.jExpand = function(){
        var element = this;

        $(element).find("tr:odd").addClass("odd");
        $(element).find("tr:not(.odd)").hide();
        $(element).find("tr:first-child").show();

        $(element).find("tr.odd").click(function() {
            $(this).next("tr").toggle();
        });

    }

    $.fn.show_all = function(){
        var element = this;
        $(element).find("tr.odd").next().show();
    }

    $.fn.hide_all = function(){
        var element = this;
        $(element).find("tr.odd").next().hide();
    }
})(jQuery);

(function($){
    $.fn.load_msa = function(id){
        var element = this;
        $.each($(element).find("tr.odd"),  function( key, value ) {

            var msa_div = "msa-" + value.id;

            var url = "get_mask?id="+id+"&marker="+value.id;

            biojs.io.fasta.parse.read(url, function(seqs){
                if(seqs){
                    div_obj = document.getElementById(msa_div);

                    var primers = div_obj.getAttribute("data-primers").toUpperCase();
                    var  primers_arr = primers.split(",");
                    var chr_index = null;

                    var i = 0;
                    seqs.map( function(item) {
                        var split = item.name.split("-");
                        if(split.length== 2){
                            item.name = split[1];
                        }
                        if(item.name == primers_arr[0]){
                            chr_index = i;
                        }
                        i++;
                    });


                    var a=seqs[0];
                    var b=seqs[1];
                    var c=b;
                    var left_most = 0;

                    if(seqs[chr_index]){
                       c=seqs[chr_index];
                    }

                    var mask = seqs[seqs.length-1];
                    var index_snp = mask.seq.indexOf("&") ;
                    if(index_snp < 0){
                        index_snp = mask.seq.indexOf(":");
                    }

                    var msa = new biojs.vis.msa.msa({
                        el: div_obj,
                        seqs: seqs,
                        zoomer: {
                            labelWidth: 100,
                            //alignmentWidth: "auto" ,
                            alignmentWidth: 1000 ,
                            labelFontsize: "13px",
                            labelIdLength: 50
                        },
                        g:{
                            conserv: false,
                            registerMouseClicks: false,
                            scheme: "nucleotide",
                            allowRectSelect : false
                        }

                    }) ;
                    msa.g.vis.set("conserv",  false);
                    msa.g.vis.set("registerMouseClicks", false);
                    msa.g.colorscheme.set("scheme", "nucleotide");
                    msa.g.config.set("registerMouseClicks", false);


                    var a_b_start = index_snp;
                    left_most = index_snp - primers_arr[1].length;
                    var end_obj = find_end_with_gaps({
                        start:index_snp,
                        length:primers_arr[1].length,
                        seq:c,
                        skip:true});

                    var a_c_end = end_obj.end -1;

                    var start_obj = find_start_with_gaps({
                        end:end_obj.end,
                        length:primers_arr[5],
                        seq:c});

                    var common_index = start_obj.start;

                    var end_obj_2 = find_end_with_gaps({
                        start:common_index,
                        length:primers_arr[3].length,
                        seq:c,
                        validate:primers_arr[3]});

                    var common_start = common_index;
                    var common_end = end_obj_2.end - 1;

                    if(primers_arr[4] == "FORWARD"){

                        start_obj = find_start_with_gaps({
                                                end:index_snp,
                                                length:primers_arr[1].length,
                                                seq:c,
                                                skip:true});
                        a_b_start =  start_obj.start + 1;
                        a_c_end = index_snp ;


                        var complement = reverse_complement(primers_arr[3]);
                        end_obj = find_end_with_gaps({
                            start:a_b_start,
                            length:primers_arr[5],
                            seq:c
                        });
                        common_index = end_obj.end;
                        var start_obj_2 = find_start_with_gaps( {
                                end:end_obj.end,
                                length:primers_arr[3].length,
                                seq:c,
                                validate: complement
                        }
                        );
                        common_start = start_obj_2.start;
                        common_end = end_obj.end - 1;
                    }
                    if(common_index >= 0 && c != b){
                        var se = new biojs.vis.msa.selection.possel(
                            {xStart: a_b_start,
                                xEnd: a_c_end
                                ,seqId: 0});

                        var se2 = new biojs.vis.msa.selection.possel(
                            {xStart: a_b_start,
                                xEnd: a_c_end
                                ,seqId: 1});

                        var se3 = new biojs.vis.msa.selection.possel(
                            {xStart: common_start,
                                xEnd: common_end
                                ,seqId: chr_index});
                        msa.g.selcol.add(se);
                        msa.g.selcol.add(se2);
                        msa.g.selcol.add(se3);
                    }
                    msa.g.zoomer.setLeftOffset(left_most);
                    msa.render();
                }
            });
        });
    }
})(jQuery);

function find_end_with_gaps(opts){
    var args = {start:0, length:0, seq:null, validate:null, skip:false} ;
    if (opts.start) args.start = opts.start;
    if (opts.length)args.length = opts.length;
    if (opts.seq)args.seq = opts.seq;
    if (opts.validate)args.validate = opts.validate;
    if (opts.skip)args.skip = opts.skip;
    var sequence = args.seq.seq.toUpperCase();
    var to_count = args.length;
    var i;

    for(i = args.start; i < sequence.length && to_count > 0; i++){
       // if(!args.skip)
        if(sequence[i] != '-'){
           to_count--;
       }
    }

    var ret ={};
    ret.sequence = sequence.substring(args.start, i)  ;
    ret.end = i;
    ret.valid = true;
    //if(args.validate){
    //    ret.valid = ret.sequence == args.validate.replace(/-/g, '');
    //}

    return ret;

}

function find_start_with_gaps(opts){
    var args = {end:50, length:0, seq:null, validate:null, skip:false}     ;
    if (opts.end) args.end = opts.end;
    if (opts.length) args.length = opts.length;
    if (opts.seq) args.seq = opts.seq;
    if (opts.validate)args.validate = opts.validate;
    if (opts.skip) args.skip = opts.skip;

    var sequence = args.seq.seq.toUpperCase();
    var to_count = args.length;
    var i;
    for(i = args.end; i > 0 && to_count > 0; i--){
       //if(!args.skip)
       if(sequence[i] != '-'  ){
           to_count--;
       }
    }

    var ret ={};
    ret.sequence = sequence.substring(i, args.end)  ;
    ret.start = i;
    ret.valid = true;
   // if(args.validate){
   //     ret.valid = ret.sequence == args.validate.replace(/-/g, '');
   // }

    return ret;

}


function reverse_complement(s) {
    var r; // Final reverse - complemented string
    var x; // nucleotide to convert
    var n; // converted nucleotide
    var i;
    var k;

    var r = ""; // Final processed string
    var i;
    var k;

    if (s.length==0)
        return ""; // Nothing to do
    // Go in reverse
    for (k=s.length-1; k>=0; k--) {
        x = s.substr(k,1);

        if (x=="a") n="t"; else
        if (x=="A") n="T"; else
        if (x=="g") n="c"; else
        if (x=="G") n="C"; else
        if (x=="c") n="g"; else
        if (x=="C") n="G"; else
        if (x=="t") n="a"; else
        if (x=="T") n="A"; else
        // RNA?
        if (x=="u") n="a"; else
        if (x=="U") n="A"; else

        // IUPAC? (see http://www.bioinformatics.org/sms/iupac.html)
        if (x=="r") n="y"; else
        if (x=="R") n="Y"; else
        if (x=="y") n="r"; else
        if (x=="Y") n="R"; else
        if (x=="k") n="m"; else
        if (x=="K") n="M"; else
        if (x=="m") n="k"; else
        if (x=="M") n="K"; else
        if (x=="b") n="v"; else
        if (x=="B") n="V"; else
        if (x=="d") n="h"; else
        if (x=="D") n="H"; else
        if (x=="h") n="d"; else
        if (x=="H") n="D"; else
        if (x=="v") n="b"; else
        if (x=="V") n="B"; else

        // Leave characters we do not understand as they are.
        // Also S and W are left unchanged.

            n = x;
        if(n.length == 1)
        r = r + n;
    }
    return r;
}