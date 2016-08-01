/**
 * Created by bianx on 29/10/2015.
 */
function getData(company) {
    jQuery('#status').html('<img src=\"images/loading_spinner.gif\"/>');
    jQuery.ajax({
        url: server_url,
        data: yr_service,
        type: "POST",
        dataType: "json",
        success: function (json) {
            var yr_data = json['results'][0]['results'];
            console.info(json);
            startPage(yr_data, false);
        }
    });

}
//
//function getSearchData(query) {
//    jQuery('#status').html('<img src=\"/yellowrust-map/images/loading_spinner.gif\"/>');
//    Fluxion.doAjax(
//        'wisControllerHelperService',
//        'getSearchData',
//        {
//            'url': ajaxurl,
//            'query': query
//        },
//        {
//            'doOnSuccess': function (json) {
//                startPage(json, true);
//            }
//        }
//    );
//
//}

function startPage(json, isCompany) {
    jQuery('#status').html('');
    for (i = 0; i < json.length; i++) {
        if (json[i]['data']['sample'] != undefined && json[i]['data']['ID']) {
            if (json[i]['data']['sample']['location']['location'] != undefined) {
                filtered_data.push(json[i]);
            }
        }
    }
    produceTable(filtered_data, isCompany);
    displayYRLocations_new(filtered_data);
    renderLegend();
}

function produceTable(data, isCompany) {
    yrtable = jQuery('#resultTable').DataTable({
        data: data,
        "columns": [
            {data: "data.ID", title: "ID", "sDefaultContent": ""},
            {data: "data.sample.Address.addressCountry", title: "Country", "sDefaultContent": ""},
            {data: "data.UKCPVS ID", title: "UKCPVS ID", "sDefaultContent": "Unknown"},
            {data: "data.sample.Disease", title: "Rust Type", "sDefaultContent": "Unknown"},
            {data: "data.sample.Name/Collector.name", title: "Collector", "sDefaultContent": ""},
            {data: "data.sample.Date collected.date", title: "Date", "sDefaultContent": ""},
            {data: "data.sample.Host", title: "Host", "sDefaultContent": ""},
            //{
            //    data: "data.sample.RNA-seq? (Selected/In progress/Completed/Failed)",
            //    title: "RNA-seq",
            //    "sDefaultContent": ""
            //},
            {
                title: "Phenotype",
                "render": function (data, type, full, meta) {
                    if (full['data']['phenotype'] != undefined && full['data']['phenotype'] != "undefined"
                        && full['data']['UKCPVS ID'] != undefined && full['data']['UKCPVS ID'] != "undefined") {
                        return phenotype_html(full['data']['UKCPVS ID'], full['data']['phenotype']);
                    }
                    else {
                        return '';
                    }
                }
            },
            {
                title: "Genotype",
                "render": function (data, type, full, meta) {
                    if (full['data']['genotype'] != undefined && full['data']['genotype'] != "undefined") {
                        if (full['data']['genotype']['Genetic group'] != undefined && full['data']['genotype']['Genetic group'] != "undefined") {
                            return full['data']['genotype']['Genetic group'];
                        }
                        else {
                            return '';
                        }
                    }
                    else {
                        return '';
                    }
                }
            },
            {data: "data.sample.Variety", title: "Variety", "sDefaultContent": ""},
            {data: "data.sample.Address.addressLocality", title: "Location", "sDefaultContent": ""},
//                {data: "data.verified", title: "Verified", "sDefaultContent": ""},
            {
                title: "Verified",
                "render": function (data, type, full, meta) {
                    return ((full["data"]["verified"]) ? 'Verified' : 'Preliminary');
                }
            },
            {
                title: "File",
                "render": function (data, type, full, meta) {
                    if (full['data']['files'] != undefined && full['data']['files'] != "undefined") {
                        return '<a href=\"http://opendata.tgac.ac.uk/field_pathogenomics/' + full['data']['files']['bam'] + '.gz\">BAM</a>';
                    }
                    else {
                        return '';
                    }
                }
            },
            {data: "data.sample_live_date.date", title: "Publish Date", "sDefaultContent": ""}

        ]
        ,
        initComplete: function () {
            var column = this.api().column(3);
            var select = jQuery('<select class="form-control"><option value="">Select Rust Type</option></select>')
                .appendTo(jQuery('#yrselect').empty())
                .on('change', function () {
                    var val = jQuery.fn.dataTable.util.escapeRegex(
                        jQuery(this).val()
                    );

                    column
                        .search(val ? '^' + val + '$' : '', true, false)
                        .draw();
                });

            column.data().unique().sort().each(function (d, j) {
                select.append('<option value="' + d + '">' + d + '</option>')
            });
        }

    });

    jQuery('#resultTable tbody').on('click', 'tr', function () {
        var data = yrtable.row(this).data();
        var la = data['data']['sample']['location']['location']['latitude'];
        var lo = data['data']['sample']['location']['location']['longitude'];
        map.setView([la, lo], 16, {animate: true});
    });

    jQuery('#resultTable').on('search.dt', function () {
        removePointers();
        var filteredData = yrtable.rows({filter: 'applied'}).data().toArray();
        displayYRLocations_new(filteredData);
    });


    jQuery("#slider").bind("valuesChanging", function (e, data) {
        datemin = Date.parse(data.values.min);
        datemax = Date.parse(data.values.max);

        yrtable.draw();
    });

    if (!isCompany) {
        yrtable.column(13).visible(false);
    }
}

jQuery.fn.dataTableExt.afnFiltering.push(
    function (oSettings, aData, iDataIndex) {
        var dateStart = datemin;
        var dateEnd = datemax;

        var evalDate = Date.parse(aData[5]);

        if (((evalDate >= dateStart && evalDate <= dateEnd) || (evalDate >= dateStart && dateEnd == 0)
            || (evalDate >= dateEnd && dateStart == 0)) || (dateStart == 0 && dateEnd == 0)) {
            return true;
        }
        else {
            return false;
        }

    });

function ukcpvs_only() {
    var column = yrtable.column(2);
    column.search('^((?!Unknown).)*$', true, false).draw();
}

function ukcpvs_and_all() {
    var column = yrtable.column(2);
    column.search('').draw();
}

function genotype_view() {
    pie_view = true;
    renderLegend();
    var column = yrtable.column(8);
    column.search('^\\d|\\d-\\d|Mixed$', true, false).draw();
}

function normal_view() {
    pie_view = false;
    renderLegend();
    var column = yrtable.column(8);
    column.search('').draw();
}

function displayYRLocations_new(array) {
    for (i = 0; i < array.length; i++) {
        var la = array[i]['data']['sample']['location']['location']['latitude'];
        var lo = array[i]['data']['sample']['location']['location']['longitude'];
        var geno = '';

        if (array[i]['data']['genotype'] != undefined && array[i]['data']['genotype'] != "undefined") {
            geno = array[i]['data']['genotype']['Genetic group'];
        }
        var collector = 'N/A';

        if (array[i]['data']['sample']['Name/Collector'] != undefined) {
            if (array[i]['data']['sample']['Name/Collector']['name'] != undefined) {
                collector = array[i]['data']['sample']['Name/Collector']['name'];
            }
        }
        var popup_note = '<b>ID: </b>' + array[i]['data']['ID'] + '<br/>'
                + '<b>Country: </b>' + array[i]['data']['sample']['Address']['addressCountry'] + '<br/>'
                + '<b>UKCPVS ID: </b>' + array[i]['data']['UKCPVS ID'] + '<br/>'
                + '<b>Rust Type: </b>' + array[i]['data']['sample']['Disease'] + '<br/>'
                + '<b>Collector: </b>' + collector + '<br/>'
                + '<b>Date collected: </b>' + array[i]['data']['sample']['Date collected']['date'] + '<br/>'
                + '<b>Host: </b>' + array[i]['data']['sample']['Host'] + '<br/>'
                + '<b>RNA-seq: </b>' + array[i]['data']['sample']['RNA-seq? (Selected/In progress/Completed/Failed)'] + '<br/>'
                + '<b>Phenotype: </b>' + phenotype_html(array[i]['data']['UKCPVS ID'], array[i]['data']['phenotype']) + '<br/>'
                + '<b>Genotype: </b>' + geno + '<br/>'
                + '<b>Town: </b>' + array[i]['data']['sample']['Address']['addressLocality']
            ;
        addPointer(la, lo, geno, popup_note);
    }
    map.addLayer(markersGroup);


}

function phenotype_html_ukid(id, phenotype) {
    if (id != undefined) {
        if (phenotype != undefined) {
            return '<u onclick="phenotype_colorbox(\'' + id + '\');" style="cursor: pointer;">' + id + '</u>';
        }
        else {
            return id;
        }
    }
    else {
        return "Unknown";
    }
}

function phenotype_html(id, phenotype) {
    if (id != undefined) {
        if (phenotype != undefined) {
            return '<u onclick="phenotype_colorbox(\'' + id + '\');" style="cursor: pointer;">' + id + '</u>';
        }
        else {
            return "";
        }
    }
    else {
        return "";
    }
}

function phenotype_colorbox(id) {
    var phynotype_data = phenotype(id);
    jQuery.colorbox({width: "80%", html: phynotype_data});
}

function phenotype(id) {
    var phynotype_data = "";
    for (i = 0; i < filtered_data.length; i++) {
        if (id == filtered_data[i]['data']['UKCPVS ID']) {
            phynotype_data = '<div style="margin:20px;">'
                + '<table id="' + id + '">'
                + '<thead><tr><th></th><th></th></tr></thead>'
                + '<tbody>'
                + '<tr><td>Batch: </td><td>' + filtered_data[i]['data']['phenotype']['Batch'] + '</td></tr>'
                + '<tr><td>No of isols tested: </td><td>' + filtered_data[i]['data']['phenotype']['No of isols tested'] + '</td></tr>'
                + '<tr><td>Isolate: </td><td>' + filtered_data[i]['data']['phenotype']['Isolate'] + '</td></tr>'
                + '<tr><td>Host: </td><td>' + filtered_data[i]['data']['phenotype']['Host'] + '</td></tr>'
                + '<tr><td>Chinese 166 Gene:1: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Chinese 166']) + '</tr>'
                + '<tr><td>Kalyansona Gene:2: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Kalyansona']) + '</tr>'
                + '<tr><td>Vilmorin 23 Gene:3a+: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Vilmorin 23']) + '</tr>'
                + '<tr><td>Nord Desprez Gene:3a+: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Nord Desprez']) + '</tr>'
                + '<tr><td>Hybrid 46 Gene:(3b)4b: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Hybrid 46']) + '</tr>'
                + '<tr><td>Heines Kolben Gene:2,6: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Heines Kolben']) + '</tr>'
                + '<tr><td>Heines Peko Gene:2,6: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Heines Peko']) + '</tr>'
                + '<tr><td>Lee Gene:7: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Lee']) + '</tr>'
                + '<tr><td>Av x Yr7 NIL Gene:7: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Av x Yr7 NIL']) + '</tr>'
                + '<tr><td>Compair Gene:8: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Compair']) + '</tr>'
                + '<tr><td>Kavkaz x 4 Fed Gene:9: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Kavkaz x 4 Fed']) + '</tr>'
                + '<tr><td>Clement Gene:9: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Clement']) + '</tr>'
                + '<tr><td>AVS x Yr 15 Gene:15: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['AVS x Yr 15']) + '</tr>'
                + '<tr><td>VPM 1 Gene:17: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['VPM 1']) + '</tr>'
                + '<tr><td>Rendezvous Gene:17: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Rendezvous']) + '</tr>'
                + '<tr><td>Av x Yr17 Gene:17: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Av x Yr17']) + '</tr>'
                + '<tr><td>Carstens V Gene:32: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Carstens V']) + '</tr>'
                + '<tr><td>Talon Gene:32: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Talon']) + '</tr>'
                + '<tr><td>Av x Yr32 Gene:32: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Av x Yr32']) + '</tr>'
                + '<tr><td>Spaldings Prolific Gene:sp: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Spaldings Prolific']) + '</tr>'
                + '<tr><td>Robigus Gene:Rob\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Robigus']) + '</tr>'
                + '<tr><td>Solstice Gene:\'Sol\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Solstice']) + '</tr>'
                + '<tr><td>Timber Gene:\'Tim\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Timber']) + '</tr>'
                + '<tr><td>Warrior Gene:War\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Warrior']) + '</tr>'
                + '<tr><td>KWS-Sterling Gene:Ste\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['KWS-Sterling']) + '</tr>'
                + '<tr><td>Cadenza Gene:6 7: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Cadenza']) + '</tr>'
                + '<tr><td>Claire Gene:Claire: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Claire']) + '</tr>'
                + '<tr><td>Crusoe Gene:Crusoe: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Crusoe']) + '</tr>'
                + '<tr><td>Ambition Gene:Amb\': </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Ambition']) + '</tr>'
                + '<tr><td>Heines VII Gene:Yr2 Yr25+: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Heines VII']) + '</tr>'
                + '<tr><td>Suwon Omar Gene:Yr(Su): </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Suwon Omar']) + '</tr>'
                + '<tr><td>Avocet Yr5 Gene:Yr5: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Avocet Yr5']) + '</tr>'
                + '<tr><td>Avocet Yr6 Gene:Yr6: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Avocet Yr6']) + '</tr>'
                + '<tr><td>Moro Gene:Yr10: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Moro']) + '</tr>'
                + '<tr><td>Avocet Yr24 Gene:Yr24: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Avocet Yr24']) + '</tr>'
                + '<tr><td>Opata Gene:Yr27+: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Opata']) + '</tr>'
                + '<tr><td>Strubes Dickkopf Gene:YrSd Yr25: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Strubes Dickkopf']) + '</tr>'
                + '<tr><td>Avocet Yr27 Gene:Yr27: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Avocet Yr27']) + '</tr>'
                + '<tr><td>Apache Gene:7 17: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Apache']) + '</tr>'
                + '<tr><td>Vuka: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Vuka']) + '</tr>'
                + '<tr><td>Grenado: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Grenado']) + '</tr>'
                + '<tr><td>Benetto: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Benetto']) + '</tr>'
                + '<tr><td>Tradiro: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Tradiro']) + '</tr>'
                + '<tr><td>Tribeca: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Tribeca']) + '</tr>'
                + '<tr><td>Tulus: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Tulus']) + '</tr>'
                + '<tr><td>Dublet: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Dublet']) + '</tr>'
                + '<tr><td>KWS Fido: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['KWS Fido']) + '</tr>'
                + '<tr><td>Brigadier: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Brigadier']) + '</tr>'
                + '<tr><td>Stigg: </td>' + divbgcolor(filtered_data[i]['data']['phenotype']['Stigg']) + '</tr>'
                + '</tbody>'
                + '</table>'
                + '</div>';

        }
    }
    return phynotype_data;
}


function divbgcolor(value) {
    if (parseFloat(value) < 1) {
        return "<td class='pheno_1 in_box'>" + value + "</td>";
    }
    if (parseFloat(value) >= 1 && parseFloat(value) < 2) {
        return "<td class='pheno_12 in_box'>" + value + "</td>";
    }
    if (parseFloat(value) >= 2 && parseFloat(value) < 3) {
        return "<td class='pheno_23 in_box'>" + value + "</td>";
    }
    if (parseFloat(value) >= 3) {
        return "<td class='pheno_3 in_box'>" + value + "</td>";
    }
    else {
        return "<td >" + value + "</td>";
    }
}

function addRandomPointer() {
    var la = randomNumberFromInterval(45, 60);
    var lo = randomNumberFromInterval(0, 25);

    addPointer(la, lo, "Hey, random! la: " + la + " lo: " + lo);
}

function addPointer(la, lo, geno, note) {
    var myClass = 'marker category-' + geno;
    var myIcon = L.divIcon({
        className: myClass,
        iconSize: null
    });
    var markerLayer;
    if (pie_view) {
        markerLayer = L.marker([la, lo], {title: geno, icon: myIcon}).bindPopup(note);
    }
    else {
        markerLayer = L.marker([la, lo], {title: geno}).bindPopup(note);
    }
    //markers.push(markerLayer);
    markersGroup.addLayer(markerLayer);

}

function removePointers() {
    map.removeLayer(markersGroup);
    if (pie_view) {
        markersGroup = new L.MarkerClusterGroup({
            maxClusterRadius: 2 * 30,
            iconCreateFunction: defineClusterIcon
        });
    }
    else {
        markersGroup = new L.MarkerClusterGroup();
    }
}

function removeTable() {
    jQuery('#resultTable').dataTable().fnDestroy();
    jQuery('#tableWrapper').html('<table id="resultTable"></table>');
}

function popup(msg) {
    L.popup()
        .setLatLng([52.621615, 8.219])
        .setContent(msg)
        .openOn(map);
}

function mapFitBounds(list) {
    map.fitBounds(list);
}

function randomNumberFromInterval(min, max) {
    return Math.random() * (max - min + 1) + min;
}

var rmax = 30;

function defineClusterIcon(cluster) {
    var children = cluster.getAllChildMarkers(),
        n = children.length, //Get number of markers in cluster
        strokeWidth = 1, //Set clusterpie stroke width
        r = rmax - 2 * strokeWidth - (n < 10 ? 12 : n < 100 ? 8 : n < 1000 ? 4 : 0), //Calculate clusterpie radius...
        iconDim = (r + strokeWidth) * 2, //...and divIcon dimensions (leaflet really want to know the size)
        data = d3.nest() //Build a dataset for the pie chart
            .key(function (d) {
                return d.options.title;
            })
            .entries(children, d3.map),
    //bake some svg markup
        html = bakeThePie({
            data: data,
            valueFunc: function (d) {
                return d.values.length;
            },
            strokeWidth: 1,
            outerRadius: r,
            innerRadius: r - 10,
            pieClass: 'cluster-pie',
            pieLabel: n,
            pieLabelClass: 'marker-cluster-pie-label',
            pathClassFunc: function (d) {
                return "category-" + d.data.key;
            }
//                    ,
//                    pathTitleFunc: function (d) {
//                        return "path title";
//                    }
        }),
    //Create a new divIcon and assign the svg markup to the html property
        myIcon = new L.DivIcon({
            html: html,
            className: 'marker-cluster',
            iconSize: new L.Point(iconDim, iconDim)
        });
    return myIcon;
}

function bakeThePie(options) {
    var data = options.data,
        valueFunc = options.valueFunc,
        r = options.outerRadius ? options.outerRadius : 28, //Default outer radius = 28px
        rInner = options.innerRadius ? options.innerRadius : r - 10, //Default inner radius = r-10
        strokeWidth = options.strokeWidth ? options.strokeWidth : 1, //Default stroke is 1
        pathClassFunc = options.pathClassFunc ? options.pathClassFunc : function () {
            return '';
        }, //Class for each path
//                pathTitleFunc = options.pathTitleFunc ? options.pathTitleFunc : function () {
//                    return '';
//                }, //Title for each path
        pieClass = options.pieClass ? options.pieClass : 'marker-cluster-pie', //Class for the whole pie
        pieLabel = options.pieLabel ? options.pieLabel : d3.sum(data, valueFunc), //Label for the whole pie
        pieLabelClass = options.pieLabelClass ? options.pieLabelClass : 'marker-cluster-pie-label',//Class for the pie label

        origo = (r + strokeWidth), //Center coordinate
        w = origo * 2, //width and height of the svg element
        h = w,
        donut = d3.layout.pie(),
        arc = d3.svg.arc().innerRadius(rInner).outerRadius(r);

    //Create an svg element
    var svg = document.createElementNS(d3.ns.prefix.svg, 'svg');
    //Create the pie chart
    var vis = d3.select(svg)
        .data([data])
        .attr('class', pieClass)
        .attr('width', w)
        .attr('height', h);

    var arcs = vis.selectAll('g.arc')
        .data(donut.value(valueFunc))
        .enter().append('svg:g')
        .attr('class', 'arc')
        .attr('transform', 'translate(' + origo + ',' + origo + ')');

    arcs.append('svg:path')
        .attr('class', pathClassFunc)
        .attr('stroke-width', strokeWidth)
        .attr('d', arc)
//                .append('svg:title')
//                .text(pathTitleFunc)
    ;
    vis.append('text')
        .attr('x', origo)
        .attr('y', origo)
        .attr('class', pieLabelClass)
        .attr('text-anchor', 'middle')
        .attr('dy', '.3em')
        .text(pieLabel);
    return serializeXmlNode(svg);
}

function serializeXmlNode(xmlNode) {
    if (typeof window.XMLSerializer != "undefined") {
        return (new window.XMLSerializer()).serializeToString(xmlNode);
    }
    else if (typeof xmlNode.xml != "undefined") {
        return xmlNode.xml;
    }
    return "";
}

function renderLegend() {
    jQuery('#legend').html('');
    if (pie_view) {
        var metajson = {
            "lookup": {
                "1": "Genetic group 1",
                "2": "Genetic group 2",
                "3": "Genetic group 3",
                "4": "Genetic group 4",
                "5": "Genetic group 5",
                "6": "Genetic group 6",
                "7": "Genetic group 7",
                "1-4": "Genetic group 1-4",
                "1-5": "Genetic group 1-5",
                "4-6": "Genetic group 4-6",
                "4-1": "Genetic group 4-1",
                "5-1": "Genetic group 5-1",
                "Mixed": "Genetic group mixed"
            }
        };

        var data = d3.entries(metajson.lookup),
            legenddiv = d3.select('#legend');

        var heading = legenddiv.append('div')
            .classed('legendheading', true)
            .text("Genotype");

        var legenditems = legenddiv.selectAll('.legenditem')
            .data(data);

        legenditems
            .enter()
            .append('div')
            .attr('class', function (d) {
                return 'category-' + d.key;
            })
            .classed({'legenditem': true})
            .text(function (d) {
                return d.value;
            });
    }
}
