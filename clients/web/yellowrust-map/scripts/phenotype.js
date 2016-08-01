/**
 * Created by bianx on 18/02/2016.
 */

function show_table(){
    phenotype_table = jQuery('#resultTable').DataTable({
        data: phenotype_data,
        scrollX: 1150,
        scrollCollapse: true,
        fixedColumns: {
            leftColumns: 4
        },
        "columns": [
            {data: "data.ID", title: "ID", "sDefaultContent": ""},
            {data: "data.sample.Variety", title: "Variety", "sDefaultContent": ""},
            {data: "data.sample.Host", title: "Host", "sDefaultContent": ""},
            {data: "data.sample.Date collected.date", title: "Date", "sDefaultContent": ""},
            {
                data: "data.phenotype.Chinese 166",
                title: "Chinese 166 Gene:1",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Kalyansona",
                title: "Kalyansona Gene:2",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Vilmorin 23",
                title: "Vilmorin 23 Gene:3a+",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Nord Desprez",
                title: "Nord Desprez Gene:3a+",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Hybrid 46", title: "Hybrid 46 Gene:(3b)4b", "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Heines Kolben",
                title: "Heines Kolben Gene:2,6",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Heines Peko",
                title: "Heines Peko Gene:2,6",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Lee",
                title: "Lee Gene:7",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Av x Yr7 NIL",
                title: "Av x Yr7 NIL Gene:7",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Compair",
                title: "Compair Gene:8",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Kavkaz x 4 Fed",
                title: "Kavkaz x 4 Fed Gene:9",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Clement",
                title: "Clement Gene:9",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.AVS x Yr 15",
                title: "AVS x Yr 15 Gene:15",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.VPM 1",
                title: "VPM 1 Gene:17",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Rendezvous",
                title: "Rendezvous Gene:17",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Av x Yr17",
                title: "Av x Yr17 Gene:17",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Carstens V",
                title: "Carstens V Gene:32",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Talon",
                title: "Talon Gene:32",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Av x Yr32",
                title: "Av x Yr32 Gene:32",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Spaldings Prolific",
                title: "Spaldings Prolific Gene:sp",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Robigus",
                title: "Robigus Gene:Rob'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Solstice",
                title: "Solstice Gene:\'Sol\'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Timber",
                title: "Timber Gene:\'Tim\'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Warrior",
                title: "Warrior Gene:War\'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.KWS-Sterling",
                title: "KWS-Sterling Gene:Ste\'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Cadenza",
                title: "Cadenza Gene:6 7",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Claire",
                title: "Claire Gene:Claire",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Crusoe",
                title: "Crusoe Gene:Crusoe",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Ambition",
                title: "Ambition Gene:Amb\'",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Heines VII",
                title: "Heines VII Gene:Yr2 Yr25+",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Suwon Omar",
                title: "Suwon Omar Gene:Yr(Su)",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Avocet Yr5",
                title: "Avocet Yr5 Gene:Yr5",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Avocet Yr6",
                title: "Avocet Yr6 Gene:Yr6",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Moro",
                title: "Moro Gene:Yr10",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Avocet Yr24",
                title: "Avocet Yr24 Gene:Yr24",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Opata",
                title: "Opata Gene:Yr27+",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Strubes Dickkopf",
                title: "Strubes Dickkopf Gene:YrSd Yr25",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Avocet Yr27",
                title: "Avocet Yr27 Gene:Yr27",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Apache",
                title: "Apache Gene:7 17",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Vuka",
                title: "Vuka",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Grenado",
                title: "Grenado",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Benetto",
                title: "Benetto",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Tradiro",
                title: "Tradiro",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Tribeca",
                title: "Tribeca",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Tulus",
                title: "Tulus",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Dublet",
                title: "Dublet",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.KWS Fido",
                title: "KWS Fido",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Brigadier",
                title: "Brigadier",
                "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            },
            {
                data: "data.phenotype.Stigg", title: "Stigg", "sDefaultContent": "",
                createdCell: function (cell, cellData, rowData, rowIndex, colIndex) {
                    color_coding(cellData, cell);
                }
            }
        ]
    });
}

function toggle_column(index) {
    var column = phenotype_table.column(index);
    column.visible(!column.visible());
}

function hide_column(index) {
    var column = phenotype_table.column(index);
    column.visible(false);
}

function show_column(index) {
    var column = phenotype_table.column(index);
    column.visible(true);
}

function color_coding(cellData, cell) {
    if (parseFloat(cellData) < 1) {
        jQuery(cell).addClass("pheno_1");
    }
    if (parseFloat(cellData) >= 1 && parseFloat(cellData) < 2) {
        jQuery(cell).addClass("pheno_12");
    }
    if (parseFloat(cellData) >= 2 && parseFloat(cellData) < 3) {
        jQuery(cell).addClass("pheno_23");
    }
    if (parseFloat(cellData) >= 3) {
        jQuery(cell).addClass("pheno_3");
    }
}