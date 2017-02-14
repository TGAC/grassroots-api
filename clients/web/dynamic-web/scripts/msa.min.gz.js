!function e(t, n, i) {
    function r(s, a) {
        if (!n[s]) {
            if (!t[s]) {
                var u = "function" == typeof require && require;
                if (!a && u)return u(s, !0);
                if (o)return o(s, !0);
                throw new Error("Cannot find module '" + s + "'")
            }
            var l = n[s] = {exports: {}};
            t[s][0].call(l.exports, function (e) {
                var n = t[s][1][e];
                return r(n ? n : e)
            }, l, l.exports, e, t, n, i)
        }
        return n[s].exports
    }

    for (var o = "function" == typeof require && require, s = 0; s < i.length; s++)r(i[s]);
    return r
}({1: [function (e, t) {
    var n = n || "undefined" != typeof navigator && navigator.msSaveOrOpenBlob && navigator.msSaveOrOpenBlob.bind(navigator) || function (e) {
        "use strict";
        if ("undefined" == typeof navigator || !/MSIE [1-9]\./.test(navigator.userAgent)) {
            var t = e.document, n = function () {
                return e.URL || e.webkitURL || e
            }, i = t.createElementNS("http://www.w3.org/1999/xhtml", "a"), r = !e.externalHost && "download"in i, o = function (n) {
                var i = t.createEvent("MouseEvents");
                i.initMouseEvent("click", !0, !1, e, 0, 0, 0, 0, 0, !1, !1, !1, !1, 0, null), n.dispatchEvent(i)
            }, s = e.webkitRequestFileSystem, a = e.requestFileSystem || s || e.mozRequestFileSystem, u = function (t) {
                (e.setImmediate || e.setTimeout)(function () {
                    throw t
                }, 0)
            }, l = "application/octet-stream", c = 0, h = [], d = function () {
                for (var e = h.length; e--;) {
                    var t = h[e];
                    "string" == typeof t ? n().revokeObjectURL(t) : t.remove()
                }
                h.length = 0
            }, f = function (e, t, n) {
                t = [].concat(t);
                for (var i = t.length; i--;) {
                    var r = e["on" + t[i]];
                    if ("function" == typeof r)try {
                        r.call(e, n || e)
                    } catch (o) {
                        u(o)
                    }
                }
            }, g = function (t, u) {
                var d, g, p, m = this, v = t.type, b = !1, y = function () {
                    var e = n().createObjectURL(t);
                    return h.push(e), e
                }, w = function () {
                    f(m, "writestart progress write writeend".split(" "))
                }, x = function () {
                    (b || !d) && (d = y(t)), g ? g.location.href = d : window.open(d, "_blank"), m.readyState = m.DONE, w()
                }, _ = function (e) {
                    return function () {
                        return m.readyState !== m.DONE ? e.apply(this, arguments) : void 0
                    }
                }, E = {create: !0, exclusive: !1};
                return m.readyState = m.INIT, u || (u = "download"), r ? (d = y(t), i.href = d, i.download = u, o(i), m.readyState = m.DONE, void w()) : (e.chrome && v && v !== l && (p = t.slice || t.webkitSlice, t = p.call(t, 0, t.size, l), b = !0), s && "download" !== u && (u += ".download"), (v === l || s) && (g = e), a ? (c += t.size, void a(e.TEMPORARY, c, _(function (e) {
                    e.root.getDirectory("saved", E, _(function (e) {
                        var n = function () {
                            e.getFile(u, E, _(function (e) {
                                e.createWriter(_(function (n) {
                                    n.onwriteend = function (t) {
                                        g.location.href = e.toURL(), h.push(e), m.readyState = m.DONE, f(m, "writeend", t)
                                    }, n.onerror = function () {
                                        var e = n.error;
                                        e.code !== e.ABORT_ERR && x()
                                    }, "writestart progress write abort".split(" ").forEach(function (e) {
                                        n["on" + e] = m["on" + e]
                                    }), n.write(t), m.abort = function () {
                                        n.abort(), m.readyState = m.DONE
                                    }, m.readyState = m.WRITING
                                }), x)
                            }), x)
                        };
                        e.getFile(u, {create: !1}, _(function (e) {
                            e.remove(), n()
                        }), _(function (e) {
                            e.code === e.NOT_FOUND_ERR ? n() : x()
                        }))
                    }), x)
                }), x)) : void x())
            }, p = g.prototype, m = function (e, t) {
                return new g(e, t)
            };
            return p.abort = function () {
                var e = this;
                e.readyState = e.DONE, f(e, "abort")
            }, p.readyState = p.INIT = 0, p.WRITING = 1, p.DONE = 2, p.error = p.onwritestart = p.onprogress = p.onwrite = p.onabort = p.onerror = p.onwriteend = null, e.addEventListener("unload", d, !1), m.unload = function () {
                d(), e.removeEventListener("unload", d, !1)
            }, m
        }
    }("undefined" != typeof self && self || "undefined" != typeof window && window || this.content);
    amdDefine = window.define, "undefined" == typeof amdDefine && "undefined" != typeof window.almond && "define"in window.almond && (amdDefine = window.almond.define), "undefined" != typeof t && null !== t ? t.exports = n : "undefined" != typeof amdDefine && null !== amdDefine && null != amdDefine.amd && amdDefine("saveAs", [], function () {
        return n
    })
}, {}], 2: [function (e, t) {
    "undefined" == typeof biojs && (t.exports = biojs = {}), "undefined" == typeof biojs.vis && (t.exports = biojs.vis = {}), window.msa = biojs.vis.msa = e("./index"), biojs.vis.easy_features = e("biojs-vis-easy_features"), "undefined" == typeof biojs.io && (biojs.io = {}), window.biojs.io.fasta = e("biojs-io-fasta"), window.biojs.io.clustal = e("biojs-io-clustal")
}, {"./index": 3, "biojs-io-clustal": 8, "biojs-io-fasta": 21, "biojs-vis-easy_features": 38}], 3: [function (e, t) {
    t.exports = e("./src/index")
}, {"./src/index": 58}], 4: [function (e, t, n) {
    !function (t, i) {
        if ("function" == typeof define && define.amd)define(["underscore", "jquery", "exports"], function (e, n, r) {
            t.Backbone = i(t, r, e, n)
        }); else if ("undefined" != typeof n) {
            var r = e("underscore");
            i(t, n, r)
        } else t.Backbone = i(t, {}, t._, t.jQuery || t.Zepto || t.ender || t.$)
    }(this, function (e, t, n, i) {
        {
            var r = e.Backbone, o = [], s = (o.push, o.slice);
            o.splice
        }
        t.VERSION = "1.1.2", t.$ = i, t.noConflict = function () {
            return e.Backbone = r, this
        }, t.emulateHTTP = !1, t.emulateJSON = !1;
        var a = t.Events = {on: function (e, t, n) {
            if (!l(this, "on", e, [t, n]) || !t)return this;
            this._events || (this._events = {});
            var i = this._events[e] || (this._events[e] = []);
            return i.push({callback: t, context: n, ctx: n || this}), this
        }, once: function (e, t, i) {
            if (!l(this, "once", e, [t, i]) || !t)return this;
            var r = this, o = n.once(function () {
                r.off(e, o), t.apply(this, arguments)
            });
            return o._callback = t, this.on(e, o, i)
        }, off: function (e, t, i) {
            var r, o, s, a, u, c, h, d;
            if (!this._events || !l(this, "off", e, [t, i]))return this;
            if (!e && !t && !i)return this._events = void 0, this;
            for (a = e ? [e] : n.keys(this._events), u = 0, c = a.length; c > u; u++)if (e = a[u], s = this._events[e]) {
                if (this._events[e] = r = [], t || i)for (h = 0, d = s.length; d > h; h++)o = s[h], (t && t !== o.callback && t !== o.callback._callback || i && i !== o.context) && r.push(o);
                r.length || delete this._events[e]
            }
            return this
        }, trigger: function (e) {
            if (!this._events)return this;
            var t = s.call(arguments, 1);
            if (!l(this, "trigger", e, t))return this;
            var n = this._events[e], i = this._events.all;
            return n && c(n, t), i && c(i, arguments), this
        }, stopListening: function (e, t, i) {
            var r = this._listeningTo;
            if (!r)return this;
            var o = !t && !i;
            i || "object" != typeof t || (i = this), e && ((r = {})[e._listenId] = e);
            for (var s in r)e = r[s], e.off(t, i, this), (o || n.isEmpty(e._events)) && delete this._listeningTo[s];
            return this
        }}, u = /\s+/, l = function (e, t, n, i) {
            if (!n)return!0;
            if ("object" == typeof n) {
                for (var r in n)e[t].apply(e, [r, n[r]].concat(i));
                return!1
            }
            if (u.test(n)) {
                for (var o = n.split(u), s = 0, a = o.length; a > s; s++)e[t].apply(e, [o[s]].concat(i));
                return!1
            }
            return!0
        }, c = function (e, t) {
            var n, i = -1, r = e.length, o = t[0], s = t[1], a = t[2];
            switch (t.length) {
                case 0:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx);
                    return;
                case 1:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o);
                    return;
                case 2:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o, s);
                    return;
                case 3:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o, s, a);
                    return;
                default:
                    for (; ++i < r;)(n = e[i]).callback.apply(n.ctx, t);
                    return
            }
        }, h = {listenTo: "on", listenToOnce: "once"};
        n.each(h, function (e, t) {
            a[t] = function (t, i, r) {
                var o = this._listeningTo || (this._listeningTo = {}), s = t._listenId || (t._listenId = n.uniqueId("l"));
                return o[s] = t, r || "object" != typeof i || (r = this), t[e](i, r, this), this
            }
        }), a.bind = a.on, a.unbind = a.off, n.extend(t, a);
        var d = t.Model = function (e, t) {
            var i = e || {};
            t || (t = {}), this.cid = n.uniqueId("c"), this.attributes = {}, t.collection && (this.collection = t.collection), t.parse && (i = this.parse(i, t) || {}), i = n.defaults({}, i, n.result(this, "defaults")), this.set(i, t), this.changed = {}, this.initialize.apply(this, arguments)
        };
        n.extend(d.prototype, a, {changed: null, validationError: null, idAttribute: "id", initialize: function () {
        }, toJSON: function () {
            return n.clone(this.attributes)
        }, sync: function () {
            return t.sync.apply(this, arguments)
        }, get: function (e) {
            return this.attributes[e]
        }, escape: function (e) {
            return n.escape(this.get(e))
        }, has: function (e) {
            return null != this.get(e)
        }, set: function (e, t, i) {
            var r, o, s, a, u, l, c, h;
            if (null == e)return this;
            if ("object" == typeof e ? (o = e, i = t) : (o = {})[e] = t, i || (i = {}), !this._validate(o, i))return!1;
            s = i.unset, u = i.silent, a = [], l = this._changing, this._changing = !0, l || (this._previousAttributes = n.clone(this.attributes), this.changed = {}), h = this.attributes, c = this._previousAttributes, this.idAttribute in o && (this.id = o[this.idAttribute]);
            for (r in o)t = o[r], n.isEqual(h[r], t) || a.push(r), n.isEqual(c[r], t) ? delete this.changed[r] : this.changed[r] = t, s ? delete h[r] : h[r] = t;
            if (!u) {
                a.length && (this._pending = i);
                for (var d = 0, f = a.length; f > d; d++)this.trigger("change:" + a[d], this, h[a[d]], i)
            }
            if (l)return this;
            if (!u)for (; this._pending;)i = this._pending, this._pending = !1, this.trigger("change", this, i);
            return this._pending = !1, this._changing = !1, this
        }, unset: function (e, t) {
            return this.set(e, void 0, n.extend({}, t, {unset: !0}))
        }, clear: function (e) {
            var t = {};
            for (var i in this.attributes)t[i] = void 0;
            return this.set(t, n.extend({}, e, {unset: !0}))
        }, hasChanged: function (e) {
            return null == e ? !n.isEmpty(this.changed) : n.has(this.changed, e)
        }, changedAttributes: function (e) {
            if (!e)return this.hasChanged() ? n.clone(this.changed) : !1;
            var t, i = !1, r = this._changing ? this._previousAttributes : this.attributes;
            for (var o in e)n.isEqual(r[o], t = e[o]) || ((i || (i = {}))[o] = t);
            return i
        }, previous: function (e) {
            return null != e && this._previousAttributes ? this._previousAttributes[e] : null
        }, previousAttributes: function () {
            return n.clone(this._previousAttributes)
        }, fetch: function (e) {
            e = e ? n.clone(e) : {}, void 0 === e.parse && (e.parse = !0);
            var t = this, i = e.success;
            return e.success = function (n) {
                return t.set(t.parse(n, e), e) ? (i && i(t, n, e), void t.trigger("sync", t, n, e)) : !1
            }, L(this, e), this.sync("read", this, e)
        }, save: function (e, t, i) {
            var r, o, s, a = this.attributes;
            if (null == e || "object" == typeof e ? (r = e, i = t) : (r = {})[e] = t, i = n.extend({validate: !0}, i), r && !i.wait) {
                if (!this.set(r, i))return!1
            } else if (!this._validate(r, i))return!1;
            r && i.wait && (this.attributes = n.extend({}, a, r)), void 0 === i.parse && (i.parse = !0);
            var u = this, l = i.success;
            return i.success = function (e) {
                u.attributes = a;
                var t = u.parse(e, i);
                return i.wait && (t = n.extend(r || {}, t)), n.isObject(t) && !u.set(t, i) ? !1 : (l && l(u, e, i), void u.trigger("sync", u, e, i))
            }, L(this, i), o = this.isNew() ? "create" : i.patch ? "patch" : "update", "patch" === o && (i.attrs = r), s = this.sync(o, this, i), r && i.wait && (this.attributes = a), s
        }, destroy: function (e) {
            e = e ? n.clone(e) : {};
            var t = this, i = e.success, r = function () {
                t.trigger("destroy", t, t.collection, e)
            };
            if (e.success = function (n) {
                (e.wait || t.isNew()) && r(), i && i(t, n, e), t.isNew() || t.trigger("sync", t, n, e)
            }, this.isNew())return e.success(), !1;
            L(this, e);
            var o = this.sync("delete", this, e);
            return e.wait || r(), o
        }, url: function () {
            var e = n.result(this, "urlRoot") || n.result(this.collection, "url") || I();
            return this.isNew() ? e : e.replace(/([^\/])$/, "$1/") + encodeURIComponent(this.id)
        }, parse: function (e) {
            return e
        }, clone: function () {
            return new this.constructor(this.attributes)
        }, isNew: function () {
            return!this.has(this.idAttribute)
        }, isValid: function (e) {
            return this._validate({}, n.extend(e || {}, {validate: !0}))
        }, _validate: function (e, t) {
            if (!t.validate || !this.validate)return!0;
            e = n.extend({}, this.attributes, e);
            var i = this.validationError = this.validate(e, t) || null;
            return i ? (this.trigger("invalid", this, i, n.extend(t, {validationError: i})), !1) : !0
        }});
        var f = ["keys", "values", "pairs", "invert", "pick", "omit"];
        n.each(f, function (e) {
            d.prototype[e] = function () {
                var t = s.call(arguments);
                return t.unshift(this.attributes), n[e].apply(n, t)
            }
        });
        var g = t.Collection = function (e, t) {
            t || (t = {}), t.model && (this.model = t.model), void 0 !== t.comparator && (this.comparator = t.comparator), this._reset(), this.initialize.apply(this, arguments), e && this.reset(e, n.extend({silent: !0}, t))
        }, p = {add: !0, remove: !0, merge: !0}, m = {add: !0, remove: !1};
        n.extend(g.prototype, a, {model: d, initialize: function () {
        }, toJSON: function (e) {
            return this.map(function (t) {
                return t.toJSON(e)
            })
        }, sync: function () {
            return t.sync.apply(this, arguments)
        }, add: function (e, t) {
            return this.set(e, n.extend({merge: !1}, t, m))
        }, remove: function (e, t) {
            var i = !n.isArray(e);
            e = i ? [e] : n.clone(e), t || (t = {});
            var r, o, s, a;
            for (r = 0, o = e.length; o > r; r++)a = e[r] = this.get(e[r]), a && (delete this._byId[a.id], delete this._byId[a.cid], s = this.indexOf(a), this.models.splice(s, 1), this.length--, t.silent || (t.index = s, a.trigger("remove", a, this, t)), this._removeReference(a, t));
            return i ? e[0] : e
        }, set: function (e, t) {
            t = n.defaults({}, t, p), t.parse && (e = this.parse(e, t));
            var i = !n.isArray(e);
            e = i ? e ? [e] : [] : n.clone(e);
            var r, o, s, a, u, l, c, h = t.at, f = this.model, g = this.comparator && null == h && t.sort !== !1, m = n.isString(this.comparator) ? this.comparator : null, v = [], b = [], y = {}, w = t.add, x = t.merge, _ = t.remove, E = !g && w && _ ? [] : !1;
            for (r = 0, o = e.length; o > r; r++) {
                if (u = e[r] || {}, s = u instanceof d ? a = u : u[f.prototype.idAttribute || "id"], l = this.get(s))_ && (y[l.cid] = !0), x && (u = u === a ? a.attributes : u, t.parse && (u = l.parse(u, t)), l.set(u, t), g && !c && l.hasChanged(m) && (c = !0)), e[r] = l; else if (w) {
                    if (a = e[r] = this._prepareModel(u, t), !a)continue;
                    v.push(a), this._addReference(a, t)
                }
                a = l || a, !E || !a.isNew() && y[a.id] || E.push(a), y[a.id] = !0
            }
            if (_) {
                for (r = 0, o = this.length; o > r; ++r)y[(a = this.models[r]).cid] || b.push(a);
                b.length && this.remove(b, t)
            }
            if (v.length || E && E.length)if (g && (c = !0), this.length += v.length, null != h)for (r = 0, o = v.length; o > r; r++)this.models.splice(h + r, 0, v[r]); else {
                E && (this.models.length = 0);
                var S = E || v;
                for (r = 0, o = S.length; o > r; r++)this.models.push(S[r])
            }
            if (c && this.sort({silent: !0}), !t.silent) {
                for (r = 0, o = v.length; o > r; r++)(a = v[r]).trigger("add", a, this, t);
                (c || E && E.length) && this.trigger("sort", this, t)
            }
            return i ? e[0] : e
        }, reset: function (e, t) {
            t || (t = {});
            for (var i = 0, r = this.models.length; r > i; i++)this._removeReference(this.models[i], t);
            return t.previousModels = this.models, this._reset(), e = this.add(e, n.extend({silent: !0}, t)), t.silent || this.trigger("reset", this, t), e
        }, push: function (e, t) {
            return this.add(e, n.extend({at: this.length}, t))
        }, pop: function (e) {
            var t = this.at(this.length - 1);
            return this.remove(t, e), t
        }, unshift: function (e, t) {
            return this.add(e, n.extend({at: 0}, t))
        }, shift: function (e) {
            var t = this.at(0);
            return this.remove(t, e), t
        }, slice: function () {
            return s.apply(this.models, arguments)
        }, get: function (e) {
            return null == e ? void 0 : this._byId[e] || this._byId[e.id] || this._byId[e.cid]
        }, at: function (e) {
            return this.models[e]
        }, where: function (e, t) {
            return n.isEmpty(e) ? t ? void 0 : [] : this[t ? "find" : "filter"](function (t) {
                for (var n in e)if (e[n] !== t.get(n))return!1;
                return!0
            })
        }, findWhere: function (e) {
            return this.where(e, !0)
        }, sort: function (e) {
            if (!this.comparator)throw new Error("Cannot sort a set without a comparator");
            return e || (e = {}), n.isString(this.comparator) || 1 === this.comparator.length ? this.models = this.sortBy(this.comparator, this) : this.models.sort(n.bind(this.comparator, this)), e.silent || this.trigger("sort", this, e), this
        }, pluck: function (e) {
            return n.invoke(this.models, "get", e)
        }, fetch: function (e) {
            e = e ? n.clone(e) : {}, void 0 === e.parse && (e.parse = !0);
            var t = e.success, i = this;
            return e.success = function (n) {
                var r = e.reset ? "reset" : "set";
                i[r](n, e), t && t(i, n, e), i.trigger("sync", i, n, e)
            }, L(this, e), this.sync("read", this, e)
        }, create: function (e, t) {
            if (t = t ? n.clone(t) : {}, !(e = this._prepareModel(e, t)))return!1;
            t.wait || this.add(e, t);
            var i = this, r = t.success;
            return t.success = function (e, n) {
                t.wait && i.add(e, t), r && r(e, n, t)
            }, e.save(null, t), e
        }, parse: function (e) {
            return e
        }, clone: function () {
            return new this.constructor(this.models)
        }, _reset: function () {
            this.length = 0, this.models = [], this._byId = {}
        }, _prepareModel: function (e, t) {
            if (e instanceof d)return e;
            t = t ? n.clone(t) : {}, t.collection = this;
            var i = new this.model(e, t);
            return i.validationError ? (this.trigger("invalid", this, i.validationError, t), !1) : i
        }, _addReference: function (e) {
            this._byId[e.cid] = e, null != e.id && (this._byId[e.id] = e), e.collection || (e.collection = this), e.on("all", this._onModelEvent, this)
        }, _removeReference: function (e) {
            this === e.collection && delete e.collection, e.off("all", this._onModelEvent, this)
        }, _onModelEvent: function (e, t, n, i) {
            ("add" !== e && "remove" !== e || n === this) && ("destroy" === e && this.remove(t, i), t && e === "change:" + t.idAttribute && (delete this._byId[t.previous(t.idAttribute)], null != t.id && (this._byId[t.id] = t)), this.trigger.apply(this, arguments))
        }});
        var v = ["forEach", "each", "map", "collect", "reduce", "foldl", "inject", "reduceRight", "foldr", "find", "detect", "filter", "select", "reject", "every", "all", "some", "any", "include", "contains", "invoke", "max", "min", "toArray", "size", "first", "head", "take", "initial", "rest", "tail", "drop", "last", "without", "difference", "indexOf", "shuffle", "lastIndexOf", "isEmpty", "chain", "sample"];
        n.each(v, function (e) {
            g.prototype[e] = function () {
                var t = s.call(arguments);
                return t.unshift(this.models), n[e].apply(n, t)
            }
        });
        var b = ["groupBy", "countBy", "sortBy", "indexBy"];
        n.each(b, function (e) {
            g.prototype[e] = function (t, i) {
                var r = n.isFunction(t) ? t : function (e) {
                    return e.get(t)
                };
                return n[e](this.models, r, i)
            }
        });
        var y = t.View = function (e) {
            this.cid = n.uniqueId("view"), e || (e = {}), n.extend(this, n.pick(e, x)), this._ensureElement(), this.initialize.apply(this, arguments), this.delegateEvents()
        }, w = /^(\S+)\s*(.*)$/, x = ["model", "collection", "el", "id", "attributes", "className", "tagName", "events"];
        n.extend(y.prototype, a, {tagName: "div", $: function (e) {
            return this.$el.find(e)
        }, initialize: function () {
        }, render: function () {
            return this
        }, remove: function () {
            return this.$el.remove(), this.stopListening(), this
        }, setElement: function (e, n) {
            return this.$el && this.undelegateEvents(), this.$el = e instanceof t.$ ? e : t.$(e), this.el = this.$el[0], n !== !1 && this.delegateEvents(), this
        }, delegateEvents: function (e) {
            if (!e && !(e = n.result(this, "events")))return this;
            this.undelegateEvents();
            for (var t in e) {
                var i = e[t];
                if (n.isFunction(i) || (i = this[e[t]]), i) {
                    var r = t.match(w), o = r[1], s = r[2];
                    i = n.bind(i, this), o += ".delegateEvents" + this.cid, "" === s ? this.$el.on(o, i) : this.$el.on(o, s, i)
                }
            }
            return this
        }, undelegateEvents: function () {
            return this.$el.off(".delegateEvents" + this.cid), this
        }, _ensureElement: function () {
            if (this.el)this.setElement(n.result(this, "el"), !1); else {
                var e = n.extend({}, n.result(this, "attributes"));
                this.id && (e.id = n.result(this, "id")), this.className && (e["class"] = n.result(this, "className"));
                var i = t.$("<" + n.result(this, "tagName") + ">").attr(e);
                this.setElement(i, !1)
            }
        }}), t.sync = function (e, i, r) {
            var o = E[e];
            n.defaults(r || (r = {}), {emulateHTTP: t.emulateHTTP, emulateJSON: t.emulateJSON});
            var s = {type: o, dataType: "json"};
            if (r.url || (s.url = n.result(i, "url") || I()), null != r.data || !i || "create" !== e && "update" !== e && "patch" !== e || (s.contentType = "application/json", s.data = JSON.stringify(r.attrs || i.toJSON(r))), r.emulateJSON && (s.contentType = "application/x-www-form-urlencoded", s.data = s.data ? {model: s.data} : {}), r.emulateHTTP && ("PUT" === o || "DELETE" === o || "PATCH" === o)) {
                s.type = "POST", r.emulateJSON && (s.data._method = o);
                var a = r.beforeSend;
                r.beforeSend = function (e) {
                    return e.setRequestHeader("X-HTTP-Method-Override", o), a ? a.apply(this, arguments) : void 0
                }
            }
            "GET" === s.type || r.emulateJSON || (s.processData = !1), "PATCH" === s.type && _ && (s.xhr = function () {
                return new ActiveXObject("Microsoft.XMLHTTP")
            });
            var u = r.xhr = t.ajax(n.extend(s, r));
            return i.trigger("request", i, u, r), u
        };
        var _ = !("undefined" == typeof window || !window.ActiveXObject || window.XMLHttpRequest && (new XMLHttpRequest).dispatchEvent), E = {create: "POST", update: "PUT", patch: "PATCH", "delete": "DELETE", read: "GET"};
        t.ajax = function () {
            return t.$.ajax.apply(t.$, arguments)
        };
        var S = t.Router = function (e) {
            e || (e = {}), e.routes && (this.routes = e.routes), this._bindRoutes(), this.initialize.apply(this, arguments)
        }, k = /\((.*?)\)/g, C = /(\(\?)?:\w+/g, j = /\*\w+/g, q = /[\-{}\[\]+?.,\\\^$|#\s]/g;
        n.extend(S.prototype, a, {initialize: function () {
        }, route: function (e, i, r) {
            n.isRegExp(e) || (e = this._routeToRegExp(e)), n.isFunction(i) && (r = i, i = ""), r || (r = this[i]);
            var o = this;
            return t.history.route(e, function (n) {
                var s = o._extractParameters(e, n);
                o.execute(r, s), o.trigger.apply(o, ["route:" + i].concat(s)), o.trigger("route", i, s), t.history.trigger("route", o, i, s)
            }), this
        }, execute: function (e, t) {
            e && e.apply(this, t)
        }, navigate: function (e, n) {
            return t.history.navigate(e, n), this
        }, _bindRoutes: function () {
            if (this.routes) {
                this.routes = n.result(this, "routes");
                for (var e, t = n.keys(this.routes); null != (e = t.pop());)this.route(e, this.routes[e])
            }
        }, _routeToRegExp: function (e) {
            return e = e.replace(q, "\\$&").replace(k, "(?:$1)?").replace(C,function (e, t) {
                return t ? e : "([^/?]+)"
            }).replace(j, "([^?]*?)"), new RegExp("^" + e + "(?:\\?([\\s\\S]*))?$")
        }, _extractParameters: function (e, t) {
            var i = e.exec(t).slice(1);
            return n.map(i, function (e, t) {
                return t === i.length - 1 ? e || null : e ? decodeURIComponent(e) : null
            })
        }});
        var z = t.History = function () {
            this.handlers = [], n.bindAll(this, "checkUrl"), "undefined" != typeof window && (this.location = window.location, this.history = window.history)
        }, N = /^[#\/]|\s+$/g, M = /^\/+|\/+$/g, O = /msie [\w.]+/, T = /\/$/, R = /#.*$/;
        z.started = !1, n.extend(z.prototype, a, {interval: 50, atRoot: function () {
            return this.location.pathname.replace(/[^\/]$/, "$&/") === this.root
        }, getHash: function (e) {
            var t = (e || this).location.href.match(/#(.*)$/);
            return t ? t[1] : ""
        }, getFragment: function (e, t) {
            if (null == e)if (this._hasPushState || !this._wantsHashChange || t) {
                e = decodeURI(this.location.pathname + this.location.search);
                var n = this.root.replace(T, "");
                e.indexOf(n) || (e = e.slice(n.length))
            } else e = this.getHash();
            return e.replace(N, "")
        }, start: function (e) {
            if (z.started)throw new Error("Backbone.history has already been started");
            z.started = !0, this.options = n.extend({root: "/"}, this.options, e), this.root = this.options.root, this._wantsHashChange = this.options.hashChange !== !1, this._wantsPushState = !!this.options.pushState, this._hasPushState = !!(this.options.pushState && this.history && this.history.pushState);
            var i = this.getFragment(), r = document.documentMode, o = O.exec(navigator.userAgent.toLowerCase()) && (!r || 7 >= r);
            if (this.root = ("/" + this.root + "/").replace(M, "/"), o && this._wantsHashChange) {
                var s = t.$('<iframe src="javascript:0" tabindex="-1">');
                this.iframe = s.hide().appendTo("body")[0].contentWindow, this.navigate(i)
            }
            this._hasPushState ? t.$(window).on("popstate", this.checkUrl) : this._wantsHashChange && "onhashchange"in window && !o ? t.$(window).on("hashchange", this.checkUrl) : this._wantsHashChange && (this._checkUrlInterval = setInterval(this.checkUrl, this.interval)), this.fragment = i;
            var a = this.location;
            if (this._wantsHashChange && this._wantsPushState) {
                if (!this._hasPushState && !this.atRoot())return this.fragment = this.getFragment(null, !0), this.location.replace(this.root + "#" + this.fragment), !0;
                this._hasPushState && this.atRoot() && a.hash && (this.fragment = this.getHash().replace(N, ""), this.history.replaceState({}, document.title, this.root + this.fragment))
            }
            return this.options.silent ? void 0 : this.loadUrl()
        }, stop: function () {
            t.$(window).off("popstate", this.checkUrl).off("hashchange", this.checkUrl), this._checkUrlInterval && clearInterval(this._checkUrlInterval), z.started = !1
        }, route: function (e, t) {
            this.handlers.unshift({route: e, callback: t})
        }, checkUrl: function () {
            var e = this.getFragment();
            return e === this.fragment && this.iframe && (e = this.getFragment(this.getHash(this.iframe))), e === this.fragment ? !1 : (this.iframe && this.navigate(e), void this.loadUrl())
        }, loadUrl: function (e) {
            return e = this.fragment = this.getFragment(e), n.any(this.handlers, function (t) {
                return t.route.test(e) ? (t.callback(e), !0) : void 0
            })
        }, navigate: function (e, t) {
            if (!z.started)return!1;
            t && t !== !0 || (t = {trigger: !!t});
            var n = this.root + (e = this.getFragment(e || ""));
            if (e = e.replace(R, ""), this.fragment !== e) {
                if (this.fragment = e, "" === e && "/" !== n && (n = n.slice(0, -1)), this._hasPushState)this.history[t.replace ? "replaceState" : "pushState"]({}, document.title, n); else {
                    if (!this._wantsHashChange)return this.location.assign(n);
                    this._updateHash(this.location, e, t.replace), this.iframe && e !== this.getFragment(this.getHash(this.iframe)) && (t.replace || this.iframe.document.open().close(), this._updateHash(this.iframe.location, e, t.replace))
                }
                return t.trigger ? this.loadUrl(e) : void 0
            }
        }, _updateHash: function (e, t, n) {
            if (n) {
                var i = e.href.replace(/(javascript:|#).*$/, "");
                e.replace(i + "#" + t)
            } else e.hash = "#" + t
        }}), t.history = new z;
        var A = function (e, t) {
            var i, r = this;
            i = e && n.has(e, "constructor") ? e.constructor : function () {
                return r.apply(this, arguments)
            }, n.extend(i, r, t);
            var o = function () {
                this.constructor = i
            };
            return o.prototype = r.prototype, i.prototype = new o, e && n.extend(i.prototype, e), i.__super__ = r.prototype, i
        };
        d.extend = g.extend = S.extend = y.extend = z.extend = A;
        var I = function () {
            throw new Error('A "url" property or function must be specified')
        }, L = function (e, t) {
            var n = t.error;
            t.error = function (i) {
                n && n(e, i, t), e.trigger("error", e, i, t)
            }
        };
        return t
    })
}, {underscore: 43}], 5: [function (e, t) {
    var n = e("backbone-events-standalone");
    n.onAll = function (e, t) {
        return this.on("all", e, t), this
    }, n.oldMixin = n.mixin, n.mixin = function (e) {
        n.oldMixin(e);
        for (var t = ["onAll"], i = 0; i < t.length; i++) {
            var r = t[i];
            e[r] = this[r]
        }
        return e
    }, t.exports = n
}, {"backbone-events-standalone": 7}], 6: [function (e, t, n) {
    !function () {
        function e() {
            return{keys: Object.keys, uniqueId: function (e) {
                var t = ++l + "";
                return e ? e + t : t
            }, has: function (e, t) {
                return a.call(e, t)
            }, each: function (e, t, n) {
                if (null != e)if (s && e.forEach === s)e.forEach(t, n); else if (e.length === +e.length) {
                    for (var i = 0, r = e.length; r > i; i++)if (t.call(n, e[i], i, e) === o)return
                } else for (var a in e)if (this.has(e, a) && t.call(n, e[a], a, e) === o)return
            }, once: function (e) {
                var t, n = !1;
                return function () {
                    return n ? t : (n = !0, t = e.apply(this, arguments), e = null, t)
                }
            }}
        }

        var i, r = this, o = {}, s = Array.prototype.forEach, a = Object.prototype.hasOwnProperty, u = Array.prototype.slice, l = 0, c = e();
        i = {on: function (e, t, n) {
            if (!d(this, "on", e, [t, n]) || !t)return this;
            this._events || (this._events = {});
            var i = this._events[e] || (this._events[e] = []);
            return i.push({callback: t, context: n, ctx: n || this}), this
        }, once: function (e, t, n) {
            if (!d(this, "once", e, [t, n]) || !t)return this;
            var i = this, r = c.once(function () {
                i.off(e, r), t.apply(this, arguments)
            });
            return r._callback = t, this.on(e, r, n)
        }, off: function (e, t, n) {
            var i, r, o, s, a, u, l, h;
            if (!this._events || !d(this, "off", e, [t, n]))return this;
            if (!e && !t && !n)return this._events = {}, this;
            for (s = e ? [e] : c.keys(this._events), a = 0, u = s.length; u > a; a++)if (e = s[a], o = this._events[e]) {
                if (this._events[e] = i = [], t || n)for (l = 0, h = o.length; h > l; l++)r = o[l], (t && t !== r.callback && t !== r.callback._callback || n && n !== r.context) && i.push(r);
                i.length || delete this._events[e]
            }
            return this
        }, trigger: function (e) {
            if (!this._events)return this;
            var t = u.call(arguments, 1);
            if (!d(this, "trigger", e, t))return this;
            var n = this._events[e], i = this._events.all;
            return n && f(n, t), i && f(i, arguments), this
        }, stopListening: function (e, t, n) {
            var i = this._listeners;
            if (!i)return this;
            var r = !t && !n;
            "object" == typeof t && (n = this), e && ((i = {})[e._listenerId] = e);
            for (var o in i)i[o].off(t, n, this), r && delete this._listeners[o];
            return this
        }};
        var h = /\s+/, d = function (e, t, n, i) {
            if (!n)return!0;
            if ("object" == typeof n) {
                for (var r in n)e[t].apply(e, [r, n[r]].concat(i));
                return!1
            }
            if (h.test(n)) {
                for (var o = n.split(h), s = 0, a = o.length; a > s; s++)e[t].apply(e, [o[s]].concat(i));
                return!1
            }
            return!0
        }, f = function (e, t) {
            var n, i = -1, r = e.length, o = t[0], s = t[1], a = t[2];
            switch (t.length) {
                case 0:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx);
                    return;
                case 1:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o);
                    return;
                case 2:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o, s);
                    return;
                case 3:
                    for (; ++i < r;)(n = e[i]).callback.call(n.ctx, o, s, a);
                    return;
                default:
                    for (; ++i < r;)(n = e[i]).callback.apply(n.ctx, t)
            }
        }, g = {listenTo: "on", listenToOnce: "once"};
        c.each(g, function (e, t) {
            i[t] = function (t, n, i) {
                var r = this._listeners || (this._listeners = {}), o = t._listenerId || (t._listenerId = c.uniqueId("l"));
                return r[o] = t, "object" == typeof n && (i = this), t[e](n, i, this), this
            }
        }), i.bind = i.on, i.unbind = i.off, i.mixin = function (e) {
            var t = ["on", "once", "off", "trigger", "stopListening", "listenTo", "listenToOnce", "bind", "unbind"];
            return c.each(t, function (t) {
                e[t] = this[t]
            }, this), e
        }, "function" == typeof define ? define(function () {
            return i
        }) : "undefined" != typeof n ? ("undefined" != typeof t && t.exports && (n = t.exports = i), n.BackboneEvents = i) : r.BackboneEvents = i
    }(this)
}, {}], 7: [function (e, t) {
    t.exports = e("./backbone-events-standalone")
}, {"./backbone-events-standalone": 6}], 8: [function (e, t) {
    var n, i, r, o, s = {}.hasOwnProperty, a = function (e, t) {
        function n() {
            this.constructor = e
        }

        for (var i in t)s.call(t, i) && (e[i] = t[i]);
        return n.prototype = t.prototype, e.prototype = new n, e.__super__ = t.prototype, e
    };
    o = e("./strings"), i = e("./generic_reader"), r = e("./seq"), t.exports = n = function (e) {
        function t() {
            return t.__super__.constructor.apply(this, arguments)
        }

        return a(t, e), t.parse = function (e) {
            var t, n, i, s, a, u, l, c, h, d;
            if (h = [], a = e.split("\n"), a[0].slice(0, 6) === !1)throw new Error("Invalid CLUSTAL Header");
            for (n = 0, t = 1, c = 0; n < a.length;)n++, s = a[n], null != s && 0 !== s.length && 0 !== s.trim().length ? o.contains(s, "*") || (1 === t && (c = 0, t = 0), l = /^(?:\s*)(\S+)(?:\s+)(\S+)(?:\s*)(\d*)(?:\s*|$)/g, u = l.exec(s), null != u ? (i = u[1], d = u[2], c >= h.length ? h.push(new r(d, i, c)) : h[c].seq += d, c++) : console.log(s)) : t = 1;
            return h
        }, t
    }(i)
}, {"./generic_reader": 9, "./seq": 10, "./strings": 11}], 9: [function (e, t) {
    var n, i;
    i = e("nets"), t.exports = n = function () {
        function e() {
        }

        return e.read = function (e, t) {
            var n;
            return n = function (e) {
                return function (n, i, r) {
                    return e._onRetrieval(r, t)
                }
            }(this), i(e, n)
        }, e._onRetrieval = function (e, t) {
            var n;
            return n = this.parse(e), t(n)
        }, e
    }()
}, {nets: 12}], 10: [function (e, t) {
    var n;
    t.exports = n = function () {
        function e(e, t, n) {
            var i;
            this.seq = e, this.name = t, this.id = n, i = {}
        }

        return e
    }()
}, {}], 11: [function (e, t) {
    var n;
    n = {contains: function (e, t) {
        return-1 !== "".indexOf.call(e, t, 0)
    }}, t.exports = n
}, {}], 12: [function (e, t) {
    function n(e, t, n) {
        i(e, t, n)
    }

    var i = e("request");
    t.exports = n
}, {request: 13}], 13: [function (e, t) {
    function n(e, t) {
        function n() {
            4 === p.readyState && E()
        }

        function r() {
            var e = null;
            if (p.response ? e = p.response : "text" !== p.responseType && p.responseType || (e = p.responseText || p.responseXML), _)try {
                e = JSON.parse(e)
            } catch (t) {
            }
            return e
        }

        function c() {
            return 1223 === p.status ? 204 : p.status
        }

        function h(e) {
            var t = null;
            if (0 === e || e >= 400 && 600 > e) {
                var n = ("string" == typeof y ? y : !1) || a[String(e).charAt(0)];
                t = new Error(n), t.statusCode = e
            }
            return t
        }

        function d() {
            var e = c(), n = h(e), i = {body: r(), statusCode: e, statusText: p.statusText, headers: s(p.getAllResponseHeaders())};
            t(n, i, i.body)
        }

        function f() {
            var e = c(), n = h(e);
            p.status = p.statusCode = e, p.body = r(), t(n, p, p.body)
        }

        function g(e) {
            t(e, p)
        }

        "string" == typeof e && (e = {uri: e}), e = e || {}, t = o(t);
        var p = e.xhr || null;
        p || (p = e.cors || e.useXDR ? new l : new u);
        var m, v = p.url = e.uri || e.url, b = p.method = e.method || "GET", y = e.body || e.data, w = p.headers = e.headers || {}, x = !!e.sync, _ = !1, E = e.response ? d : f;
        if ("json"in e && (_ = !0, w.Accept = "application/json", "GET" !== b && "HEAD" !== b && (w["Content-Type"] = "application/json", y = JSON.stringify(e.json))), p.onreadystatechange = n, p.onload = E, p.onerror = g, p.onprogress = function () {
        }, p.ontimeout = i, p.open(b, v, !x), (e.withCredentials || e.cors && e.withCredentials !== !1) && (p.withCredentials = !0), x || (p.timeout = "timeout"in e ? e.timeout : 5e3), p.setRequestHeader)for (m in w)w.hasOwnProperty(m) && p.setRequestHeader(m, w[m]); else if (e.headers)throw new Error("Headers cannot be set on an XDomainRequest object");
        return"responseType"in e && (p.responseType = e.responseType), "beforeSend"in e && "function" == typeof e.beforeSend && e.beforeSend(p), p.send(y), p
    }

    function i() {
    }

    var r = e("global/window"), o = e("once"), s = e("parse-headers"), a = {0: "Internal XMLHttpRequest Error", 4: "4xx Client Error", 5: "5xx Server Error"}, u = r.XMLHttpRequest || i, l = "withCredentials"in new u ? u : r.XDomainRequest;
    t.exports = n
}, {"global/window": 14, once: 15, "parse-headers": 19}], 14: [function (e, t) {
    (function (e) {
        t.exports = "undefined" != typeof window ? window : "undefined" != typeof e ? e : {}
    }).call(this, "undefined" != typeof self ? self : "undefined" != typeof window ? window : {})
}, {}], 15: [function (e, t) {
    function n(e) {
        var t = !1;
        return function () {
            return t ? void 0 : (t = !0, e.apply(this, arguments))
        }
    }

    t.exports = n, n.proto = n(function () {
        Object.defineProperty(Function.prototype, "once", {value: function () {
            return n(this)
        }, configurable: !0})
    })
}, {}], 16: [function (e, t) {
    function n(e, t, n) {
        if (!s(t))throw new TypeError("iterator must be a function");
        arguments.length < 3 && (n = this), "[object Array]" === a.call(e) ? i(e, t, n) : "string" == typeof e ? r(e, t, n) : o(e, t, n)
    }

    function i(e, t, n) {
        for (var i = 0, r = e.length; r > i; i++)u.call(e, i) && t.call(n, e[i], i, e)
    }

    function r(e, t, n) {
        for (var i = 0, r = e.length; r > i; i++)t.call(n, e.charAt(i), i, e)
    }

    function o(e, t, n) {
        for (var i in e)u.call(e, i) && t.call(n, e[i], i, e)
    }

    var s = e("is-function");
    t.exports = n;
    var a = Object.prototype.toString, u = Object.prototype.hasOwnProperty
}, {"is-function": 17}], 17: [function (e, t) {
    function n(e) {
        var t = i.call(e);
        return"[object Function]" === t || "function" == typeof e && "[object RegExp]" !== t || "undefined" != typeof window && (e === window.setTimeout || e === window.alert || e === window.confirm || e === window.prompt)
    }

    t.exports = n;
    var i = Object.prototype.toString
}, {}], 18: [function (e, t, n) {
    function i(e) {
        return e.replace(/^\s*|\s*$/g, "")
    }

    n = t.exports = i, n.left = function (e) {
        return e.replace(/^\s*/, "")
    }, n.right = function (e) {
        return e.replace(/\s*$/, "")
    }
}, {}], 19: [function (e, t) {
    var n = e("trim"), i = e("for-each");
    t.exports = function (e) {
        if (!e)return{};
        var t = {};
        return i(n(e).split("\n"), function (e) {
            var i = e.indexOf(":");
            t[n(e.slice(0, i)).toLowerCase()] = n(e.slice(i + 1))
        }), t
    }
}, {"for-each": 16, trim: 18}], 20: [function (e, t) {
    t.exports = e(9)
}, {nets: 28}], 21: [function (e, t) {
    t.exports.parse = e("./parser"), t.exports.writer = e("./writer")
}, {"./parser": 22, "./writer": 25}], 22: [function (e, t) {
    var n, i, r, o, s = {}.hasOwnProperty, a = function (e, t) {
        function n() {
            this.constructor = e
        }

        for (var i in t)s.call(t, i) && (e[i] = t[i]);
        return n.prototype = t.prototype, e.prototype = new n, e.__super__ = t.prototype, e
    };
    o = e("./strings"), i = e("./generic_reader"), r = e("biojs-model").seq, t.exports = n = function (e) {
        function t() {
            return t.__super__.constructor.apply(this, arguments)
        }

        return a(t, e), t.parse = function (e) {
            var t, n, i, s, a, u, l, c, h, d, f;
            for (c = [], f = e.split("\n"), h = 0, d = f.length; d > h; h++)if (l = f[h], ">" === l[0] || ";" === l[0]) {
                if (u = l.slice(1), t = new r("", u, c.length), c.push(t), o.contains("|", l)) {
                    for (s = u.split("|"), a = 1; a < s.length;)n = s[a], i = s[a + 1], t.meta[n] = i, a += 2;
                    t.name = s[s.length - 1]
                }
            } else t.seq += l;
            return c
        }, t
    }(i)
}, {"./generic_reader": 20, "./strings": 23, "biojs-model": 26}], 23: [function (e, t) {
    t.exports = e(11)
}, {}], 24: [function (e, t) {
    var n;
    n = {}, n.splitNChars = function (e, t) {
        var n, i, r, o;
        for (i = [], n = r = 0, o = e.length - 1; t > 0 ? o >= r : r >= o; n = r += t)i.push(e.substr(n, t));
        return i
    }, t.exports = n
}, {}], 25: [function (e, t) {
    var n, i;
    i = e("./utils"), t.exports = n = function () {
        function e() {
        }

        return e["export"] = function (e, t) {
            var n, r, o, s;
            for (r = "", o = 0, s = e.length; s > o; o++)n = e[o], null != t && (n = t(n)), r += ">" + n.name + "\n", r += i.splitNChars(n.seq, 80).join("\n"), r += "\n";
            return r
        }, e
    }()
}, {"./utils": 24}], 26: [function (e, t) {
    t.exports.seq = e("./seq")
}, {"./seq": 27}], 27: [function (e, t) {
    t.exports = function (e, t, n) {
        this.seq = e, this.name = t, this.id = n, this.meta = {}
    }
}, {}], 28: [function (e, t) {
    t.exports = e(12)
}, {request: 29}], 29: [function (e, t) {
    t.exports = e(13)
}, {"global/window": 30, once: 31, "parse-headers": 35}], 30: [function (e, t) {
    t.exports = e(14)
}, {}], 31: [function (e, t) {
    t.exports = e(15)
}, {}], 32: [function (e, t) {
    t.exports = e(16)
}, {"is-function": 33}], 33: [function (e, t) {
    t.exports = e(17)
}, {}], 34: [function (e, t) {
    t.exports = e(18)
}, {}], 35: [function (e, t) {
    t.exports = e(19)
}, {"for-each": 32, trim: 34}], 36: [function (e, t) {
    t.exports = e(26)
}, {"./seq": 37}], 37: [function (e, t) {
    t.exports = e(27)
}, {}], 38: [function (e, t) {
    t.exports.model = e("./model"), t.exports.stage = e("./stage")
}, {"./model": 39, "./stage": 40}], 39: [function (e, t) {
    var n;
    t.exports = n = function () {
        function e(e, t, n, i) {
            this.xStart = e, this.xEnd = t, this.text = n, this.fillColor = i
        }

        return e.prototype.xStart = -1, e.prototype.xEnd = -1, e.prototype.height = -1, e.prototype.text = "", e.prototype.fillColor = "red", e.prototype.fillOpacity = .5, e.prototype.type = "rectangle", e.prototype.borderSize = 1, e.prototype.borderColor = "black", e.prototype.borderOpacity = .5, e
    }()
}, {}], 40: [function (e, t) {
    var n, i, r;
    r = e("./utils"), n = e("biojs-events"), t.exports = i = function () {
        function e(t, i, r, o, s, a) {
            var u, l;
            n.mixin(e.prototype), l = "string" == typeof t ? document.getElementById(t) : t, null == r && (r = 10), null == o && (o = 18), null == s && (s = void 0), null == a && (a = 0), u = this.create(l, i, r, o, s, a)
        }

        return e.prototype.create = function (t, n, i, r, o, s) {
            var a, u, l, c, h, d, f, g, p, m, v, b, y, w, x, _, E;
            if (null != n && (null != n ? n.length : void 0) > 0) {
                for (null == o && (o = n.reduce(function (e, t) {
                    return Math.max(e, t.end, 0)
                })), n = e.sortFeatureArray(n), p = e.getMinRows(n, o), g = function () {
                    var e, t;
                    for (t = [], m = e = 1; p >= 1 ? p >= e : e >= p; m = p >= 1 ? ++e : --e)t.push(document.createDocumentFragment());
                    return t
                }(), v = 0, w = n.length; w > v; v++)for (a = n[v], l = this.drawRectangle(a, i, r), b = 0, x = g.length; x > b; b++)if (h = g[b], d = h.childNodes.length, 0 === d || (null != (E = h.childNodes[d - 1]) ? E.xEnd : void 0) < a.xStart) {
                    d > 0 ? l.style.marginLeft = "" + i * (a.xStart - 1 - h.childNodes[d - 1].xEnd) + "px" : 0 === d && (l.style.marginLeft = "" + i * a.xStart + "px"), h.appendChild(l);
                    break
                }
                for (c = document.createDocumentFragment(), y = 0, _ = g.length; _ > y; y++)h = g[y], f = document.createElement("span"), f.className = "biojs-easy-feature-row", f.style.display = "block", null != s && (u = document.createElement("span"), u.style.width = "" + s + "px", f.appendChild(u)), f.appendChild(h), c.appendChild(f);
                t.className = "biojs-easy-feature", t.style.display = "block", t.appendChild(c)
            }
            return t
        }, e.prototype.drawRectangle = function (e, t, n) {
            var i, o, s, a;
            return s = document.createElement("span"), s.textContent = (null != (a = e.text) ? a.length : void 0) > 0 ? e.text : "#", i = r.hex2rgb(e.fillColor), s.style.backgroundColor = r.rgba(i, e.fillOpacity), s.bgColor = i, o = e.borderColor, s.style.border = "" + e.borderSize + "px solid " + o, s.style.width = "" + t * (e.xEnd - e.xStart + 1) + "px", s.style.fontSize = "" + n + "px", s.xEnd = e.xEnd, s.style.display = "inline-block", s.addEventListener("mouseover", function (t) {
                return function (n) {
                    var i, o;
                    return o = .8, i = n.target, i.style.backgroundColor = r.rgba(i.bgColor, o), t.trigger("mouseover", e)
                }
            }(this)), s.addEventListener("mouseout", function (t) {
                return function () {
                    var n, i;
                    return i = .5, n = event.target, n.style.backgroundColor = r.rgba(n.bgColor, i), t.trigger("mouseover", e)
                }
            }(this)), s.addEventListener("click", function (t) {
                return function () {
                    return t.trigger("click", e)
                }
            }(this)), s
        }, e.sortFeatureArray = function (e) {
            var t;
            return t = function (e, t) {
                return e.xStart < t.xStart ? -1 : e.xStart > t.xStart ? 1 : void 0
            }, e.sort(t)
        }, e.getMinRows = function (e, t) {
            var n, i, r, o, s, a, u, l, c, h, d;
            for (r = function () {
                var e, n;
                for (n = [], o = e = 1; t >= 1 ? t >= e : e >= t; o = t >= 1 ? ++e : --e)n.push(0);
                return n
            }(), s = 0, l = e.length; l > s; s++)for (n = e[s], o = a = c = n.xStart, h = n.xEnd; h >= a; o = a += 1)r[o]++;
            for (i = 0, o = u = 0, d = t - 1; d >= u; o = u += 1)r[o] > i && (i = r[o]);
            return i
        }, e
    }()
}, {"./utils": 41, "biojs-events": 5}], 41: [function (e, t) {
    var n;
    n = {}, n.rgb = function (e, t, i) {
        return"undefined" == typeof t ? n.rgb(e.r, e.g, e.b) : ["rgb(", [e || 0, t || 0, i || 0].join(","), ")"].join()
    }, n.rgba = function (e, t, i, r) {
        return"undefined" == typeof i ? n.rgba(e.r, e.g, e.b, t) : ["rgba(", [e || 0, t || 0, i || 0, r || 1].join(","), ")"].join("")
    }, n.hex2rgb = function (e) {
        var t, i, r, o, s;
        return s = n.string2hex(e), null != s && (e = s), i = parseInt(e, 16), isNaN(i) ? void 0 : (o = i >> 16 & 255, r = i >> 8 & 255, t = 255 & i, {r: o, g: r, b: t})
    }, n.string2hex = function (e) {
        var t;
        return t = {aliceblue: "f0f8ff", antiquewhite: "faebd7", aqua: "00ffff", aquamarine: "7fffd4", azure: "f0ffff", beige: "f5f5dc", bisque: "ffe4c4", black: "000000", blanchedalmond: "ffebcd", blue: "0000ff", blueviolet: "8a2be2", brown: "a52a2a", burlywood: "deb887", cadetblue: "5f9ea0", chartreuse: "7fff00", chocolate: "d2691e", coral: "ff7f50", cornflowerblue: "6495ed", cornsilk: "fff8dc", crimson: "dc143c", cyan: "00ffff", darkblue: "00008b", darkcyan: "008b8b", darkgoldenrod: "b8860b", darkgray: "a9a9a9", darkgreen: "006400", darkkhaki: "bdb76b", darkmagenta: "8b008b", darkolivegreen: "556b2f", darkorange: "ff8c00", darkorchid: "9932cc", darkred: "8b0000", darksalmon: "e9967a", darkseagreen: "8fbc8f", darkslateblue: "483d8b", darkslategray: "2f4f4f", darkturquoise: "00ced1", darkviolet: "9400d3", deeppink: "ff1493", deepskyblue: "00bfff", dimgray: "696969", dodgerblue: "1e90ff", firebrick: "b22222", floralwhite: "fffaf0", forestgreen: "228b22", fuchsia: "ff00ff", gainsboro: "dcdcdc", ghostwhite: "f8f8ff", gold: "ffd700", goldenrod: "daa520", gray: "808080", green: "008000", greenyellow: "adff2f", honeydew: "f0fff0", hotpink: "ff69b4", "indianred ": "cd5c5c", indigo: "4b0082", ivory: "fffff0", khaki: "f0e68c", lavender: "e6e6fa", lavenderblush: "fff0f5", lawngreen: "7cfc00", lemonchiffon: "fffacd", lightblue: "add8e6", lightcoral: "f08080", lightcyan: "e0ffff", lightgoldenrodyellow: "fafad2", lightgrey: "d3d3d3", lightgreen: "90ee90", lightpink: "ffb6c1", lightsalmon: "ffa07a", lightseagreen: "20b2aa", lightskyblue: "87cefa", lightslategray: "778899", lightsteelblue: "b0c4de", lightyellow: "ffffe0", lime: "00ff00", limegreen: "32cd32", linen: "faf0e6", magenta: "ff00ff", maroon: "800000", mediumaquamarine: "66cdaa", mediumblue: "0000cd", mediumorchid: "ba55d3", mediumpurple: "9370d8", mediumseagreen: "3cb371", mediumslateblue: "7b68ee", mediumspringgreen: "00fa9a", mediumturquoise: "48d1cc", mediumvioletred: "c71585", midnightblue: "191970", mintcream: "f5fffa", mistyrose: "ffe4e1", moccasin: "ffe4b5", navajowhite: "ffdead", navy: "000080", oldlace: "fdf5e6", olive: "808000", olivedrab: "6b8e23", orange: "ffa500", orangered: "ff4500", orchid: "da70d6", palegoldenrod: "eee8aa", palegreen: "98fb98", paleturquoise: "afeeee", palevioletred: "d87093", papayawhip: "ffefd5", peachpuff: "ffdab9", peru: "cd853f", pink: "ffc0cb", plum: "dda0dd", powderblue: "b0e0e6", purple: "800080", red: "ff0000", rosybrown: "bc8f8f", royalblue: "4169e1", saddlebrown: "8b4513", salmon: "fa8072", sandybrown: "f4a460", seagreen: "2e8b57", seashell: "fff5ee", sienna: "a0522d", silver: "c0c0c0", skyblue: "87ceeb", slateblue: "6a5acd", slategray: "708090", snow: "fffafa", springgreen: "00ff7f", steelblue: "4682b4", tan: "d2b48c", teal: "008080", thistle: "d8bfd8", tomato: "ff6347", turquoise: "40e0d0", violet: "ee82ee", wheat: "f5deb3", white: "ffffff", whitesmoke: "f5f5f5", yellow: "ffff00", yellowgreen: "9acd32"}, null != t[e.toLowerCase()] ? t[e.toLowerCase()] : void 0
    }, n.rgb2hex = function (e) {
        return e = e.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/), "#" + ("0" + parseInt(e[1], 10).toString(16)).slice(-2) + ("0" + parseInt(e[2], 10).toString(16)).slice(-2) + ("0" + parseInt(e[3], 10).toString(16)).slice(-2)
    }, t.exports = n
}, {}], 42: [function (e, t) {
    !function (e) {
        function n(t) {
            var n = t.length, i = typeof t;
            return p(i) || t === e ? !1 : 1 === t.nodeType && n ? !0 : m(i) || 0 === n || "number" == typeof n && n > 0 && n - 1 in t
        }

        function i(e, t) {
            var n, i;
            this.originalEvent = e, i = function (e, t) {
                this[e] = "preventDefault" === e ? function () {
                    return this.defaultPrevented = !0, t[e]()
                } : p(t[e]) ? function () {
                    return t[e]()
                } : t[e]
            };
            for (n in e)(e[n] || "function" == typeof e[n]) && i.call(this, n, e);
            v.extend(this, t)
        }

        var r, o = e.$, s = e.jBone, a = /^<(\w+)\s*\/?>$/, u = /^(?:[^#<]*(<[\w\W]+>)[^>]*$|#([\w\-]*)$)/, l = [].slice, c = [].splice, h = Object.keys, d = document, f = function (e) {
            return"string" == typeof e
        }, g = function (e) {
            return e instanceof Object
        }, p = function (e) {
            var t = {};
            return e && "[object Function]" === t.toString.call(e)
        }, m = function (e) {
            return Array.isArray(e)
        }, v = function (e, t) {
            return new r.init(e, t)
        };
        v.noConflict = function () {
            return e.$ = o, e.jBone = s, v
        }, r = v.fn = v.prototype = {init: function (e, t) {
            var n, i, r, o;
            if (!e)return this;
            if (f(e)) {
                if (i = a.exec(e))return this[0] = d.createElement(i[1]), this.length = 1, g(t) && this.attr(t), this;
                if ((i = u.exec(e)) && i[1]) {
                    for (o = d.createDocumentFragment(), r = d.createElement("div"), r.innerHTML = e; r.lastChild;)o.appendChild(r.firstChild);
                    return n = l.call(o.childNodes), v.merge(this, n)
                }
                if (v.isElement(t))return v(t).find(e);
                try {
                    return n = d.querySelectorAll(e), v.merge(this, n)
                } catch (s) {
                    return this
                }
            }
            return e.nodeType ? (this[0] = e, this.length = 1, this) : p(e) ? e() : e instanceof v ? e : v.makeArray(e, this)
        }, pop: [].pop, push: [].push, reverse: [].reverse, shift: [].shift, sort: [].sort, splice: [].splice, slice: [].slice, indexOf: [].indexOf, forEach: [].forEach, unshift: [].unshift, concat: [].concat, join: [].join, every: [].every, some: [].some, filter: [].filter, map: [].map, reduce: [].reduce, reduceRight: [].reduceRight, length: 0}, r.constructor = v, r.init.prototype = r, v.setId = function (t) {
            var n = t.jid;
            t === e ? n = "window" : void 0 === t.jid && (t.jid = n = ++v._cache.jid), v._cache.events[n] || (v._cache.events[n] = {})
        }, v.getData = function (t) {
            t = t instanceof v ? t[0] : t;
            var n = t === e ? "window" : t.jid;
            return{jid: n, events: v._cache.events[n]}
        }, v.isElement = function (e) {
            return e && e instanceof v || e instanceof HTMLElement || f(e)
        }, v._cache = {events: {}, jid: 0}, v.merge = function (e, t) {
            for (var n = t.length, i = e.length, r = 0; n > r;)e[i++] = t[r++];
            return e.length = i, e
        }, v.contains = function (e, t) {
            var n;
            return e.reverse().some(function (e) {
                return e.contains(t) ? n = e : void 0
            }), n
        }, v.extend = function (e) {
            var t, n, i, r;
            return c.call(arguments, 1).forEach(function (o) {
                if (o)for (t = h(o), n = t.length, i = 0, r = e; n > i; i++)r[t[i]] = o[t[i]]
            }), e
        }, v.makeArray = function (e, t) {
            var i = t || [];
            return null !== e && (n(e) ? v.merge(i, f(e) ? [e] : e) : i.push(e)), i
        }, v.Event = function (e, t) {
            var n, i;
            return e.type && !t && (t = e, e = e.type), n = e.split(".").splice(1).join("."), i = e.split(".")[0], e = d.createEvent("Event"), e.initEvent(i, !0, !0), v.extend(e, {namespace: n, isDefaultPrevented: function () {
                return e.defaultPrevented
            }}, t)
        }, r.on = function (e) {
            var t, n, r, o, s, a, u, l, c = arguments, h = this.length, d = 0;
            for (2 === c.length ? t = c[1] : (n = c[1], t = c[2]), l = function (l) {
                v.setId(l), s = v.getData(l).events, e.split(" ").forEach(function (e) {
                    a = e.split(".")[0], r = e.split(".").splice(1).join("."), s[a] = s[a] || [], o = function (e) {
                        e.namespace && e.namespace !== r || (u = null, n ? (~v(l).find(n).indexOf(e.target) || (u = v.contains(v(l).find(n), e.target))) && (u = u || e.target, e = new i(e, {currentTarget: u}), t.call(u, e)) : t.call(l, e))
                    }, s[a].push({namespace: r, fn: o, originfn: t}), l.addEventListener && l.addEventListener(a, o, !1)
                })
            }; h > d; d++)l(this[d]);
            return this
        }, r.one = function (e) {
            var t, n, i, r = arguments, o = 0, s = this.length;
            for (2 === r.length ? t = r[1] : (n = r[1], t = r[2]), i = function (i) {
                e.split(" ").forEach(function (e) {
                    var r = function (n) {
                        v(i).off(e, r), t.call(i, n)
                    };
                    n ? v(i).on(e, n, r) : v(i).on(e, r)
                })
            }; s > o; o++)i(this[o]);
            return this
        }, r.trigger = function (e) {
            var t, n = [], i = 0, r = this.length;
            if (!e)return this;
            for (f(e) ? n = e.split(" ").map(function (e) {
                return v.Event(e)
            }) : (e = e instanceof Event ? e : v.Event(e), n = [e]), t = function (e) {
                n.forEach(function (t) {
                    t.type && e.dispatchEvent && e.dispatchEvent(t)
                })
            }; r > i; i++)t(this[i]);
            return this
        }, r.off = function (e, t) {
            var n, i, r, o, s = 0, a = this.length, u = function (e, n, i, r, o) {
                var s;
                (t && o.originfn === t || !t) && (s = o.fn), e[n][i].fn === s && (r.removeEventListener(n, s), v._cache.events[v.getData(r).jid][n].splice(i, 1))
            };
            for (r = function (t) {
                var r, s, a;
                return(n = v.getData(t).events) ? !e && n ? h(n).forEach(function (e) {
                    for (s = n[e], r = s.length; r--;)u(n, e, r, t, s[r])
                }) : void e.split(" ").forEach(function (e) {
                    if (o = e.split(".")[0], i = e.split(".").splice(1).join("."), n[o])for (s = n[o], r = s.length; r--;)a = s[r], (!i || i && a.namespace === i) && u(n, o, r, t, a); else i && h(n).forEach(function (e) {
                        for (s = n[e], r = s.length; r--;)a = s[r], a.namespace.split(".")[0] === i.split(".")[0] && u(n, e, r, t, a)
                    })
                }) : void 0
            }; a > s; s++)r(this[s]);
            return this
        }, r.find = function (e) {
            for (var t = [], n = 0, i = this.length, r = function (n) {
                p(n.querySelectorAll) && [].forEach.call(n.querySelectorAll(e), function (e) {
                    t.push(e)
                })
            }; i > n; n++)r(this[n]);
            return v(t)
        }, r.get = function (e) {
            return this[e]
        }, r.eq = function (e) {
            return v(this[e])
        }, r.parent = function () {
            for (var e, t = [], n = 0, i = this.length; i > n; n++)!~t.indexOf(e = this[n].parentElement) && e && t.push(e);
            return v(t)
        }, r.toArray = function () {
            return l.call(this)
        }, r.is = function () {
            var e = arguments;
            return this.some(function (t) {
                return t.tagName.toLowerCase() === e[0]
            })
        }, r.has = function () {
            var e = arguments;
            return this.some(function (t) {
                return t.querySelectorAll(e[0]).length
            })
        }, r.attr = function (e, t) {
            var n, i = arguments, r = 0, o = this.length;
            if (f(e) && 1 === i.length)return this[0] && this[0].getAttribute(e);
            for (2 === i.length ? n = function (n) {
                n.setAttribute(e, t)
            } : g(e) && (n = function (t) {
                h(e).forEach(function (n) {
                    t.setAttribute(n, e[n])
                })
            }); o > r; r++)n(this[r]);
            return this
        }, r.val = function (e) {
            var t = 0, n = this.length;
            if (0 === arguments.length)return this[0] && this[0].value;
            for (; n > t; t++)this[t].value = e;
            return this
        }, r.css = function (t, n) {
            var i, r = arguments, o = 0, s = this.length;
            if (f(t) && 1 === r.length)return this[0] && e.getComputedStyle(this[0])[t];
            for (2 === r.length ? i = function (e) {
                e.style[t] = n
            } : g(t) && (i = function (e) {
                h(t).forEach(function (n) {
                    e.style[n] = t[n]
                })
            }); s > o; o++)i(this[o]);
            return this
        }, r.data = function (e, t) {
            var n, i = arguments, r = {}, o = 0, s = this.length, a = function (e, t, n) {
                g(n) ? (e.jdata = e.jdata || {}, e.jdata[t] = n) : e.dataset[t] = n
            }, u = function (e) {
                return"true" === e ? !0 : "false" === e ? !1 : e
            };
            if (0 === i.length)return this[0].jdata && (r = this[0].jdata), h(this[0].dataset).forEach(function (e) {
                r[e] = u(this[0].dataset[e])
            }, this), r;
            if (1 === i.length && f(e))return this[0] && u(this[0].dataset[e] || this[0].jdata && this[0].jdata[e]);
            for (1 === i.length && g(e) ? n = function (t) {
                h(e).forEach(function (n) {
                    a(t, n, e[n])
                })
            } : 2 === i.length && (n = function (n) {
                a(n, e, t)
            }); s > o; o++)n(this[o]);
            return this
        }, r.removeData = function (e) {
            for (var t, n, i = 0, r = this.length; r > i; i++)if (t = this[i].jdata, n = this[i].dataset, e)t && t[e] && delete t[e], delete n[e]; else {
                for (e in t)delete t[e];
                for (e in n)delete n[e]
            }
            return this
        }, r.html = function (e) {
            var t, n = arguments;
            return 1 === n.length && void 0 !== e ? this.empty().append(e) : 0 === n.length && (t = this[0]) ? t.innerHTML : this
        }, r.append = function (e) {
            var t, n = 0, i = this.length;
            for (f(e) && u.exec(e) ? e = v(e) : g(e) || (e = document.createTextNode(e)), e = e instanceof v ? e : v(e), t = function (t, n) {
                e.forEach(function (e) {
                    t.appendChild(n ? e.cloneNode() : e)
                })
            }; i > n; n++)t(this[n], n);
            return this
        }, r.appendTo = function (e) {
            return v(e).append(this), this
        }, r.empty = function () {
            for (var e, t = 0, n = this.length; n > t; t++)for (e = this[t]; e.lastChild;)e.removeChild(e.lastChild);
            return this
        }, r.remove = function () {
            var e, t = 0, n = this.length;
            for (this.off(); n > t; t++)e = this[t], delete e.jdata, e.parentNode && e.parentNode.removeChild(e);
            return this
        }, "object" == typeof t && t && "object" == typeof t.exports ? t.exports = v : "function" == typeof define && define.amd ? (define(function () {
            return v
        }), e.jBone = e.$ = v) : "object" == typeof e && "object" == typeof e.document && (e.jBone = e.$ = v)
    }(window)
}, {}], 43: [function (e, t, n) {
    (function () {
        var e = this, i = e._, r = {}, o = Array.prototype, s = Object.prototype, a = Function.prototype, u = o.push, l = o.slice, c = o.concat, h = s.toString, d = s.hasOwnProperty, f = o.forEach, g = o.map, p = o.reduce, m = o.reduceRight, v = o.filter, b = o.every, y = o.some, w = o.indexOf, x = o.lastIndexOf, _ = Array.isArray, E = Object.keys, S = a.bind, k = function (e) {
            return e instanceof k ? e : this instanceof k ? void(this._wrapped = e) : new k(e)
        };
        "undefined" != typeof n ? ("undefined" != typeof t && t.exports && (n = t.exports = k), n._ = k) : e._ = k, k.VERSION = "1.6.0";
        var C = k.each = k.forEach = function (e, t, n) {
            if (null == e)return e;
            if (f && e.forEach === f)e.forEach(t, n); else if (e.length === +e.length) {
                for (var i = 0, o = e.length; o > i; i++)if (t.call(n, e[i], i, e) === r)return
            } else for (var s = k.keys(e), i = 0, o = s.length; o > i; i++)if (t.call(n, e[s[i]], s[i], e) === r)return;
            return e
        };
        k.map = k.collect = function (e, t, n) {
            var i = [];
            return null == e ? i : g && e.map === g ? e.map(t, n) : (C(e, function (e, r, o) {
                i.push(t.call(n, e, r, o))
            }), i)
        };
        var j = "Reduce of empty array with no initial value";
        k.reduce = k.foldl = k.inject = function (e, t, n, i) {
            var r = arguments.length > 2;
            if (null == e && (e = []), p && e.reduce === p)return i && (t = k.bind(t, i)), r ? e.reduce(t, n) : e.reduce(t);
            if (C(e, function (e, o, s) {
                r ? n = t.call(i, n, e, o, s) : (n = e, r = !0)
            }), !r)throw new TypeError(j);
            return n
        }, k.reduceRight = k.foldr = function (e, t, n, i) {
            var r = arguments.length > 2;
            if (null == e && (e = []), m && e.reduceRight === m)return i && (t = k.bind(t, i)), r ? e.reduceRight(t, n) : e.reduceRight(t);
            var o = e.length;
            if (o !== +o) {
                var s = k.keys(e);
                o = s.length
            }
            if (C(e, function (a, u, l) {
                u = s ? s[--o] : --o, r ? n = t.call(i, n, e[u], u, l) : (n = e[u], r = !0)
            }), !r)throw new TypeError(j);
            return n
        }, k.find = k.detect = function (e, t, n) {
            var i;
            return q(e, function (e, r, o) {
                return t.call(n, e, r, o) ? (i = e, !0) : void 0
            }), i
        }, k.filter = k.select = function (e, t, n) {
            var i = [];
            return null == e ? i : v && e.filter === v ? e.filter(t, n) : (C(e, function (e, r, o) {
                t.call(n, e, r, o) && i.push(e)
            }), i)
        }, k.reject = function (e, t, n) {
            return k.filter(e, function (e, i, r) {
                return!t.call(n, e, i, r)
            }, n)
        }, k.every = k.all = function (e, t, n) {
            t || (t = k.identity);
            var i = !0;
            return null == e ? i : b && e.every === b ? e.every(t, n) : (C(e, function (e, o, s) {
                return(i = i && t.call(n, e, o, s)) ? void 0 : r
            }), !!i)
        };
        var q = k.some = k.any = function (e, t, n) {
            t || (t = k.identity);
            var i = !1;
            return null == e ? i : y && e.some === y ? e.some(t, n) : (C(e, function (e, o, s) {
                return i || (i = t.call(n, e, o, s)) ? r : void 0
            }), !!i)
        };
        k.contains = k.include = function (e, t) {
            return null == e ? !1 : w && e.indexOf === w ? -1 != e.indexOf(t) : q(e, function (e) {
                return e === t
            })
        }, k.invoke = function (e, t) {
            var n = l.call(arguments, 2), i = k.isFunction(t);
            return k.map(e, function (e) {
                return(i ? t : e[t]).apply(e, n)
            })
        }, k.pluck = function (e, t) {
            return k.map(e, k.property(t))
        }, k.where = function (e, t) {
            return k.filter(e, k.matches(t))
        }, k.findWhere = function (e, t) {
            return k.find(e, k.matches(t))
        }, k.max = function (e, t, n) {
            if (!t && k.isArray(e) && e[0] === +e[0] && e.length < 65535)return Math.max.apply(Math, e);
            var i = -1 / 0, r = -1 / 0;
            return C(e, function (e, o, s) {
                var a = t ? t.call(n, e, o, s) : e;
                a > r && (i = e, r = a)
            }), i
        }, k.min = function (e, t, n) {
            if (!t && k.isArray(e) && e[0] === +e[0] && e.length < 65535)return Math.min.apply(Math, e);
            var i = 1 / 0, r = 1 / 0;
            return C(e, function (e, o, s) {
                var a = t ? t.call(n, e, o, s) : e;
                r > a && (i = e, r = a)
            }), i
        }, k.shuffle = function (e) {
            var t, n = 0, i = [];
            return C(e, function (e) {
                t = k.random(n++), i[n - 1] = i[t], i[t] = e
            }), i
        }, k.sample = function (e, t, n) {
            return null == t || n ? (e.length !== +e.length && (e = k.values(e)), e[k.random(e.length - 1)]) : k.shuffle(e).slice(0, Math.max(0, t))
        };
        var z = function (e) {
            return null == e ? k.identity : k.isFunction(e) ? e : k.property(e)
        };
        k.sortBy = function (e, t, n) {
            return t = z(t), k.pluck(k.map(e,function (e, i, r) {
                return{value: e, index: i, criteria: t.call(n, e, i, r)}
            }).sort(function (e, t) {
                var n = e.criteria, i = t.criteria;
                if (n !== i) {
                    if (n > i || void 0 === n)return 1;
                    if (i > n || void 0 === i)return-1
                }
                return e.index - t.index
            }), "value")
        };
        var N = function (e) {
            return function (t, n, i) {
                var r = {};
                return n = z(n), C(t, function (o, s) {
                    var a = n.call(i, o, s, t);
                    e(r, a, o)
                }), r
            }
        };
        k.groupBy = N(function (e, t, n) {
            k.has(e, t) ? e[t].push(n) : e[t] = [n]
        }), k.indexBy = N(function (e, t, n) {
            e[t] = n
        }), k.countBy = N(function (e, t) {
            k.has(e, t) ? e[t]++ : e[t] = 1
        }), k.sortedIndex = function (e, t, n, i) {
            n = z(n);
            for (var r = n.call(i, t), o = 0, s = e.length; s > o;) {
                var a = o + s >>> 1;
                n.call(i, e[a]) < r ? o = a + 1 : s = a
            }
            return o
        }, k.toArray = function (e) {
            return e ? k.isArray(e) ? l.call(e) : e.length === +e.length ? k.map(e, k.identity) : k.values(e) : []
        }, k.size = function (e) {
            return null == e ? 0 : e.length === +e.length ? e.length : k.keys(e).length
        }, k.first = k.head = k.take = function (e, t, n) {
            return null == e ? void 0 : null == t || n ? e[0] : 0 > t ? [] : l.call(e, 0, t)
        }, k.initial = function (e, t, n) {
            return l.call(e, 0, e.length - (null == t || n ? 1 : t))
        }, k.last = function (e, t, n) {
            return null == e ? void 0 : null == t || n ? e[e.length - 1] : l.call(e, Math.max(e.length - t, 0))
        }, k.rest = k.tail = k.drop = function (e, t, n) {
            return l.call(e, null == t || n ? 1 : t)
        }, k.compact = function (e) {
            return k.filter(e, k.identity)
        };
        var M = function (e, t, n) {
            return t && k.every(e, k.isArray) ? c.apply(n, e) : (C(e, function (e) {
                k.isArray(e) || k.isArguments(e) ? t ? u.apply(n, e) : M(e, t, n) : n.push(e)
            }), n)
        };
        k.flatten = function (e, t) {
            return M(e, t, [])
        }, k.without = function (e) {
            return k.difference(e, l.call(arguments, 1))
        }, k.partition = function (e, t) {
            var n = [], i = [];
            return C(e, function (e) {
                (t(e) ? n : i).push(e)
            }), [n, i]
        }, k.uniq = k.unique = function (e, t, n, i) {
            k.isFunction(t) && (i = n, n = t, t = !1);
            var r = n ? k.map(e, n, i) : e, o = [], s = [];
            return C(r, function (n, i) {
                (t ? i && s[s.length - 1] === n : k.contains(s, n)) || (s.push(n), o.push(e[i]))
            }), o
        }, k.union = function () {
            return k.uniq(k.flatten(arguments, !0))
        }, k.intersection = function (e) {
            var t = l.call(arguments, 1);
            return k.filter(k.uniq(e), function (e) {
                return k.every(t, function (t) {
                    return k.contains(t, e)
                })
            })
        }, k.difference = function (e) {
            var t = c.apply(o, l.call(arguments, 1));
            return k.filter(e, function (e) {
                return!k.contains(t, e)
            })
        }, k.zip = function () {
            for (var e = k.max(k.pluck(arguments, "length").concat(0)), t = new Array(e), n = 0; e > n; n++)t[n] = k.pluck(arguments, "" + n);
            return t
        }, k.object = function (e, t) {
            if (null == e)return{};
            for (var n = {}, i = 0, r = e.length; r > i; i++)t ? n[e[i]] = t[i] : n[e[i][0]] = e[i][1];
            return n
        }, k.indexOf = function (e, t, n) {
            if (null == e)return-1;
            var i = 0, r = e.length;
            if (n) {
                if ("number" != typeof n)return i = k.sortedIndex(e, t), e[i] === t ? i : -1;
                i = 0 > n ? Math.max(0, r + n) : n
            }
            if (w && e.indexOf === w)return e.indexOf(t, n);
            for (; r > i; i++)if (e[i] === t)return i;
            return-1
        }, k.lastIndexOf = function (e, t, n) {
            if (null == e)return-1;
            var i = null != n;
            if (x && e.lastIndexOf === x)return i ? e.lastIndexOf(t, n) : e.lastIndexOf(t);
            for (var r = i ? n : e.length; r--;)if (e[r] === t)return r;
            return-1
        }, k.range = function (e, t, n) {
            arguments.length <= 1 && (t = e || 0, e = 0), n = arguments[2] || 1;
            for (var i = Math.max(Math.ceil((t - e) / n), 0), r = 0, o = new Array(i); i > r;)o[r++] = e, e += n;
            return o
        };
        var O = function () {
        };
        k.bind = function (e, t) {
            var n, i;
            if (S && e.bind === S)return S.apply(e, l.call(arguments, 1));
            if (!k.isFunction(e))throw new TypeError;
            return n = l.call(arguments, 2), i = function () {
                if (!(this instanceof i))return e.apply(t, n.concat(l.call(arguments)));
                O.prototype = e.prototype;
                var r = new O;
                O.prototype = null;
                var o = e.apply(r, n.concat(l.call(arguments)));
                return Object(o) === o ? o : r
            }
        }, k.partial = function (e) {
            var t = l.call(arguments, 1);
            return function () {
                for (var n = 0, i = t.slice(), r = 0, o = i.length; o > r; r++)i[r] === k && (i[r] = arguments[n++]);
                for (; n < arguments.length;)i.push(arguments[n++]);
                return e.apply(this, i)
            }
        }, k.bindAll = function (e) {
            var t = l.call(arguments, 1);
            if (0 === t.length)throw new Error("bindAll must be passed function names");
            return C(t, function (t) {
                e[t] = k.bind(e[t], e)
            }), e
        }, k.memoize = function (e, t) {
            var n = {};
            return t || (t = k.identity), function () {
                var i = t.apply(this, arguments);
                return k.has(n, i) ? n[i] : n[i] = e.apply(this, arguments)
            }
        }, k.delay = function (e, t) {
            var n = l.call(arguments, 2);
            return setTimeout(function () {
                return e.apply(null, n)
            }, t)
        }, k.defer = function (e) {
            return k.delay.apply(k, [e, 1].concat(l.call(arguments, 1)))
        }, k.throttle = function (e, t, n) {
            var i, r, o, s = null, a = 0;
            n || (n = {});
            var u = function () {
                a = n.leading === !1 ? 0 : k.now(), s = null, o = e.apply(i, r), i = r = null
            };
            return function () {
                var l = k.now();
                a || n.leading !== !1 || (a = l);
                var c = t - (l - a);
                return i = this, r = arguments, 0 >= c ? (clearTimeout(s), s = null, a = l, o = e.apply(i, r), i = r = null) : s || n.trailing === !1 || (s = setTimeout(u, c)), o
            }
        }, k.debounce = function (e, t, n) {
            var i, r, o, s, a, u = function () {
                var l = k.now() - s;
                t > l ? i = setTimeout(u, t - l) : (i = null, n || (a = e.apply(o, r), o = r = null))
            };
            return function () {
                o = this, r = arguments, s = k.now();
                var l = n && !i;
                return i || (i = setTimeout(u, t)), l && (a = e.apply(o, r), o = r = null), a
            }
        }, k.once = function (e) {
            var t, n = !1;
            return function () {
                return n ? t : (n = !0, t = e.apply(this, arguments), e = null, t)
            }
        }, k.wrap = function (e, t) {
            return k.partial(t, e)
        }, k.compose = function () {
            var e = arguments;
            return function () {
                for (var t = arguments, n = e.length - 1; n >= 0; n--)t = [e[n].apply(this, t)];
                return t[0]
            }
        }, k.after = function (e, t) {
            return function () {
                return--e < 1 ? t.apply(this, arguments) : void 0
            }
        }, k.keys = function (e) {
            if (!k.isObject(e))return[];
            if (E)return E(e);
            var t = [];
            for (var n in e)k.has(e, n) && t.push(n);
            return t
        }, k.values = function (e) {
            for (var t = k.keys(e), n = t.length, i = new Array(n), r = 0; n > r; r++)i[r] = e[t[r]];
            return i
        }, k.pairs = function (e) {
            for (var t = k.keys(e), n = t.length, i = new Array(n), r = 0; n > r; r++)i[r] = [t[r], e[t[r]]];
            return i
        }, k.invert = function (e) {
            for (var t = {}, n = k.keys(e), i = 0, r = n.length; r > i; i++)t[e[n[i]]] = n[i];
            return t
        }, k.functions = k.methods = function (e) {
            var t = [];
            for (var n in e)k.isFunction(e[n]) && t.push(n);
            return t.sort()
        }, k.extend = function (e) {
            return C(l.call(arguments, 1), function (t) {
                if (t)for (var n in t)e[n] = t[n]
            }), e
        }, k.pick = function (e) {
            var t = {}, n = c.apply(o, l.call(arguments, 1));
            return C(n, function (n) {
                n in e && (t[n] = e[n])
            }), t
        }, k.omit = function (e) {
            var t = {}, n = c.apply(o, l.call(arguments, 1));
            for (var i in e)k.contains(n, i) || (t[i] = e[i]);
            return t
        }, k.defaults = function (e) {
            return C(l.call(arguments, 1), function (t) {
                if (t)for (var n in t)void 0 === e[n] && (e[n] = t[n])
            }), e
        }, k.clone = function (e) {
            return k.isObject(e) ? k.isArray(e) ? e.slice() : k.extend({}, e) : e
        }, k.tap = function (e, t) {
            return t(e), e
        };
        var T = function (e, t, n, i) {
            if (e === t)return 0 !== e || 1 / e == 1 / t;
            if (null == e || null == t)return e === t;
            e instanceof k && (e = e._wrapped), t instanceof k && (t = t._wrapped);
            var r = h.call(e);
            if (r != h.call(t))return!1;
            switch (r) {
                case"[object String]":
                    return e == String(t);
                case"[object Number]":
                    return e != +e ? t != +t : 0 == e ? 1 / e == 1 / t : e == +t;
                case"[object Date]":
                case"[object Boolean]":
                    return+e == +t;
                case"[object RegExp]":
                    return e.source == t.source && e.global == t.global && e.multiline == t.multiline && e.ignoreCase == t.ignoreCase
            }
            if ("object" != typeof e || "object" != typeof t)return!1;
            for (var o = n.length; o--;)if (n[o] == e)return i[o] == t;
            var s = e.constructor, a = t.constructor;
            if (s !== a && !(k.isFunction(s) && s instanceof s && k.isFunction(a) && a instanceof a) && "constructor"in e && "constructor"in t)return!1;
            n.push(e), i.push(t);
            var u = 0, l = !0;
            if ("[object Array]" == r) {
                if (u = e.length, l = u == t.length)for (; u-- && (l = T(e[u], t[u], n, i)););
            } else {
                for (var c in e)if (k.has(e, c) && (u++, !(l = k.has(t, c) && T(e[c], t[c], n, i))))break;
                if (l) {
                    for (c in t)if (k.has(t, c) && !u--)break;
                    l = !u
                }
            }
            return n.pop(), i.pop(), l
        };
        k.isEqual = function (e, t) {
            return T(e, t, [], [])
        }, k.isEmpty = function (e) {
            if (null == e)return!0;
            if (k.isArray(e) || k.isString(e))return 0 === e.length;
            for (var t in e)if (k.has(e, t))return!1;
            return!0
        }, k.isElement = function (e) {
            return!(!e || 1 !== e.nodeType)
        }, k.isArray = _ || function (e) {
            return"[object Array]" == h.call(e)
        }, k.isObject = function (e) {
            return e === Object(e)
        }, C(["Arguments", "Function", "String", "Number", "Date", "RegExp"], function (e) {
            k["is" + e] = function (t) {
                return h.call(t) == "[object " + e + "]"
            }
        }), k.isArguments(arguments) || (k.isArguments = function (e) {
            return!(!e || !k.has(e, "callee"))
        }), "function" != typeof/./ && (k.isFunction = function (e) {
            return"function" == typeof e
        }), k.isFinite = function (e) {
            return isFinite(e) && !isNaN(parseFloat(e))
        }, k.isNaN = function (e) {
            return k.isNumber(e) && e != +e
        }, k.isBoolean = function (e) {
            return e === !0 || e === !1 || "[object Boolean]" == h.call(e)
        }, k.isNull = function (e) {
            return null === e
        }, k.isUndefined = function (e) {
            return void 0 === e
        }, k.has = function (e, t) {
            return d.call(e, t)
        }, k.noConflict = function () {
            return e._ = i, this
        }, k.identity = function (e) {
            return e
        }, k.constant = function (e) {
            return function () {
                return e
            }
        }, k.property = function (e) {
            return function (t) {
                return t[e]
            }
        }, k.matches = function (e) {
            return function (t) {
                if (t === e)return!0;
                for (var n in e)if (e[n] !== t[n])return!1;
                return!0
            }
        }, k.times = function (e, t, n) {
            for (var i = Array(Math.max(0, e)), r = 0; e > r; r++)i[r] = t.call(n, r);
            return i
        }, k.random = function (e, t) {
            return null == t && (t = e, e = 0), e + Math.floor(Math.random() * (t - e + 1))
        }, k.now = Date.now || function () {
            return(new Date).getTime()
        };
        var R = {escape: {"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#x27;"}};
        R.unescape = k.invert(R.escape);
        var A = {escape: new RegExp("[" + k.keys(R.escape).join("") + "]", "g"), unescape: new RegExp("(" + k.keys(R.unescape).join("|") + ")", "g")};
        k.each(["escape", "unescape"], function (e) {
            k[e] = function (t) {
                return null == t ? "" : ("" + t).replace(A[e], function (t) {
                    return R[e][t]
                })
            }
        }), k.result = function (e, t) {
            if (null == e)return void 0;
            var n = e[t];
            return k.isFunction(n) ? n.call(e) : n
        }, k.mixin = function (e) {
            C(k.functions(e), function (t) {
                var n = k[t] = e[t];
                k.prototype[t] = function () {
                    var e = [this._wrapped];
                    return u.apply(e, arguments), F.call(this, n.apply(k, e))
                }
            })
        };
        var I = 0;
        k.uniqueId = function (e) {
            var t = ++I + "";
            return e ? e + t : t
        }, k.templateSettings = {evaluate: /<%([\s\S]+?)%>/g, interpolate: /<%=([\s\S]+?)%>/g, escape: /<%-([\s\S]+?)%>/g};
        var L = /(.)^/, H = {"'": "'", "\\": "\\", "\r": "r", "\n": "n", "	": "t", "\u2028": "u2028", "\u2029": "u2029"}, W = /\\|'|\r|\n|\t|\u2028|\u2029/g;
        k.template = function (e, t, n) {
            var i;
            n = k.defaults({}, n, k.templateSettings);
            var r = new RegExp([(n.escape || L).source, (n.interpolate || L).source, (n.evaluate || L).source].join("|") + "|$", "g"), o = 0, s = "__p+='";
            e.replace(r, function (t, n, i, r, a) {
                return s += e.slice(o, a).replace(W, function (e) {
                    return"\\" + H[e]
                }), n && (s += "'+\n((__t=(" + n + "))==null?'':_.escape(__t))+\n'"), i && (s += "'+\n((__t=(" + i + "))==null?'':__t)+\n'"), r && (s += "';\n" + r + "\n__p+='"), o = a + t.length, t
            }), s += "';\n", n.variable || (s = "with(obj||{}){\n" + s + "}\n"), s = "var __t,__p='',__j=Array.prototype.join,print=function(){__p+=__j.call(arguments,'');};\n" + s + "return __p;\n";
            try {
                i = new Function(n.variable || "obj", "_", s)
            } catch (a) {
                throw a.source = s, a
            }
            if (t)return i(t, k);
            var u = function (e) {
                return i.call(this, e, k)
            };
            return u.source = "function(" + (n.variable || "obj") + "){\n" + s + "}", u
        }, k.chain = function (e) {
            return k(e).chain()
        };
        var F = function (e) {
            return this._chain ? k(e).chain() : e
        };
        k.mixin(k), C(["pop", "push", "reverse", "shift", "sort", "splice", "unshift"], function (e) {
            var t = o[e];
            k.prototype[e] = function () {
                var n = this._wrapped;
                return t.apply(n, arguments), "shift" != e && "splice" != e || 0 !== n.length || delete n[0], F.call(this, n)
            }
        }), C(["concat", "join", "slice"], function (e) {
            var t = o[e];
            k.prototype[e] = function () {
                return F.call(this, t.apply(this._wrapped, arguments))
            }
        }), k.extend(k.prototype, {chain: function () {
            return this._chain = !0, this
        }, value: function () {
            return this._wrapped
        }}), "function" == typeof define && define.amd && define("underscore", [], function () {
            return k
        })
    }).call(this)
}, {}], 44: [function (e, t) {
    var n;
    n = e("underscore"), t.exports = function (e) {
        var t;
        return e = e.map(function (e) {
            return e.get("seq")
        }), t = new Array(e.length), n.each(e, function (e) {
            return n.each(e, function (e, n) {
                return null == t[n] && (t[n] = {}), null == t[n][e] && (t[n][e] = 0), t[n][e]++
            })
        }), n.reduce(t, function (e, t) {
            var i;
            return i = n.keys(t), e += n.max(i, function (e) {
                return t[e]
            })
        }, "")
    }
}, {underscore: 43}], 45: [function (e, t) {
    var n;
    t.exports = n = function (e, t) {
        return e.each(function (e) {
            var n, i, r, o, s, a;
            for (r = e.get("seq"), i = 0, o = 0, n = s = 0, a = r.length - 1; a >= 0 ? a >= s : s >= a; n = a >= 0 ? ++s : --s)"-" !== r[n] && "-" !== t[n] && (o++, r[n] === t[n] && i++);
            return e.set("identity", i / o)
        })
    }
}, {}], 46: [function (e, t) {
    t.exports.consensus = e("./ConsensusCalc")
}, {"./ConsensusCalc": 44}], 47: [function (e, t) {
    {
        var n = e("underscore");
        t.exports = function (e, t) {
            var i, r = this;
            i = e && n.has(e, "constructor") ? e.constructor : function () {
                return r.apply(this, arguments)
            }, n.extend(i, r, t);
            var o = function () {
                this.constructor = i
            };
            return o.prototype = r.prototype, i.prototype = new o, e && n.extend(i.prototype, e), i.__super__ = r.prototype, i
        }
    }
}, {underscore: 43}], 48: [function (e, t) {
    var n, i, r;
    i = function (e) {
        var t, n;
        n = [];
        for (t in e)n.push(t);
        return n.sort(function () {
            return function (t, n) {
                var i, r;
                return i = e[t].ordering, r = e[n].ordering, r > i ? -1 : i > r ? 1 : 0
            }
        }(this)), n
    }, r = e("../bone/view"), n = r.extend({renderSubviews: function () {
        var e, t, n, r, o, s, a, u, l, c;
        for (o = this.el, e = document.createElement("div"), this.setElement(e), t = document.createDocumentFragment(), null != o.parentNode && o.parentNode.replaceChild(this.el, o), a = this._views(), u = i(a), l = 0, c = u.length; c > l; l++)n = u[l], s = a[n], s.render(), r = a[n].el, null != r && t.appendChild(r);
        return e.appendChild(t), e
    }, addView: function (e, t) {
        var n;
        if (n = this._views(), null == t)throw"Invalid plugin. ";
        return null == t.ordering && (t.ordering = e), n[e] = t
    }, removeViews: function () {
        var e, t, n;
        n = this._views();
        for (t in n)e = n[t], e.undelegateEvents(), e.unbind(), null != e.removeViews && e.removeViews(), e.remove();
        return this.views = {}
    }, removeView: function (e) {
        var t;
        return t = this._views(), t[e].remove(), delete t[e]
    }, getView: function (e) {
        var t;
        return t = this._views(), t[e]
    }, remove: function () {
        return this.removeViews(), r.prototype.remove.apply(this)
    }, _views: function () {
        return null == this.views && (this.views = {}), this.views
    }}), t.exports = n
}, {"../bone/view": 49}], 49: [function (e, t) {
    var n = e("underscore"), i = e("biojs-events"), r = e("./extend"), o = e("jbone"), s = function (e) {
        this.cid = n.uniqueId("view"), e || (e = {}), n.extend(this, n.pick(e, u)), this._ensureElement(), this.initialize.apply(this, arguments)
    }, a = /^(\S+)\s*(.*)$/, u = ["model", "collection", "el", "id", "attributes", "className", "tagName", "events"];
    n.extend(s.prototype, i, {tagName: "div", $: function (e) {
        return this.$el.find(e)
    }, initialize: function () {
    }, render: function () {
        return this
    }, remove: function () {
        return this._removeElement(), this.stopListening(), this
    }, _removeElement: function () {
        this.$el.remove()
    }, setElement: function (e) {
        return this.undelegateEvents(), this._setElement(e), this.delegateEvents(), this
    }, _setElement: function (e) {
        this.$el = e instanceof o ? e : o(e), this.el = this.$el[0]
    }, delegateEvents: function (e) {
        if (!e && !(e = n.result(this, "events")))return this;
        this.undelegateEvents();
        for (var t in e) {
            var i = e[t];
            if (n.isFunction(i) || (i = this[e[t]]), i) {
                var r = t.match(a);
                this.delegate(r[1], r[2], n.bind(i, this))
            }
        }
        return this
    }, delegate: function (e, t, n) {
        this.$el.on(e + ".delegateEvents" + this.cid, t, n)
    }, undelegateEvents: function () {
        return this.$el && this.$el.off(".delegateEvents" + this.cid), this
    }, undelegate: function (e, t, n) {
        this.$el.off(e + ".delegateEvents" + this.cid, t, n)
    }, _createElement: function (e) {
        return document.createElement(e)
    }, _ensureElement: function () {
        if (this.el)this.setElement(n.result(this, "el")); else {
            var e = n.extend({}, n.result(this, "attributes"));
            this.id && (e.id = n.result(this, "id")), this.className && (e["class"] = n.result(this, "className")), this.setElement(this._createElement(n.result(this, "tagName"))), this._setAttributes(e)
        }
    }, _setAttributes: function (e) {
        this.$el.attr(e)
    }}), s.extend = r, t.exports = s
}, {"./extend": 47, "biojs-events": 5, jbone: 42, underscore: 43}], 50: [function (e, t) {
    var n, i;
    i = e("backbone").Model, t.exports = n = i.extend({defaults: {scheme: "taylor", colorBackground: !0, showLowerCase: !0}})
}, {backbone: 4}], 51: [function (e, t) {
    var n, i, r, o;
    i = e("backbone").Model, r = e("../algo/ConsensusCalc"), o = e("underscore"), t.exports = n = i.extend({defaults: {scaling: "lin"}, initialize: function () {
        return null == this.get("hidden") ? this.set("hidden", []) : void 0
    }, _calcConservationPre: function (e) {
        var t, n, i, s;
        return t = r(e), e = e.map(function (e) {
            return e.get("seq")
        }), i = o.max(e,function (e) {
            return e.length
        }).length, s = new Array(i), n = new Array(i), o.each(e, function (e) {
            return o.each(e, function (e, i) {
                return s[i] = s[i] + 1 || 1, t[i] === e ? n[i] = n[i] + 1 || 1 : void 0
            })
        }), [n, s, i]
    }, calcConservation: function (e) {
        return"exp" === this.attributes.scaling ? this.calcConservationExp(e) : "log" === this.attributes.scaling ? this.calcConservationLog(e) : "lin" === this.attributes.scaling ? this.calcConservationLin(e) : void 0
    }, calcConservationLin: function (e) {
        var t, n, i, r, o, s, a;
        for (s = this._calcConservationPre(e), n = s[0], r = s[1], i = s[2], t = o = 0, a = i - 1; a >= 0 ? a >= o : o >= a; t = a >= 0 ? ++o : --o)n[t] = n[t] / r[t];
        return this.set("conserv", n), n
    }, calcConservationLog: function (e) {
        var t, n, i, r, o, s, a;
        for (s = this._calcConservationPre(e), n = s[0], r = s[1], i = s[2], t = o = 0, a = i - 1; a >= 0 ? a >= o : o >= a; t = a >= 0 ? ++o : --o)n[t] = Math.log(n[t] + 1) / Math.log(r[t] + 1);
        return this.set("conserv", n), n
    }, calcConservationExp: function (e) {
        var t, n, i, r, o, s, a;
        for (s = this._calcConservationPre(e), n = s[0], r = s[1], i = s[2], t = o = 0, a = i - 1; a >= 0 ? a >= o : o >= a; t = a >= 0 ? ++o : --o)n[t] = Math.exp(n[t] + 1) / Math.exp(r[t] + 1);
        return this.set("conserv", n), n
    }})
}, {"../algo/ConsensusCalc": 44, backbone: 4, underscore: 43}], 52: [function (e, t) {
    var n, i;
    i = e("backbone").Model, t.exports = n = i.extend({defaults: {registerMouseEvents: !1, autofit: !0}})
}, {backbone: 4}], 53: [function (e, t) {
    var n, i, r;
    i = e("backbone").Model, r = e("../algo/ConsensusCalc"), t.exports = n = i.extend({defaults: {consenus: ""}, getConsensus: function (e) {
        var t;
        return t = r(e), this.set("consenus", t), t
    }})
}, {"../algo/ConsensusCalc": 44, backbone: 4}], 54: [function (e, t) {
    var n, i, r, o, s, a;
    a = e("underscore"), i = e("backbone").Model, s = i.extend({defaults: {type: "super"}}), o = s.extend({defaults: a.extend({}, s.prototype.defaults, {type: "row", seqId: ""}), inRow: function (e) {
        return e === this.get("seqId")
    }, inColumn: function () {
        return!0
    }}), n = s.extend({defaults: a.extend({}, s.prototype.defaults, {type: "column", xStart: -1, xEnd: -1}), inRow: function () {
        return!0
    }, inColumn: function (e) {
        return e >= xStart && xEnd >= e
    }}), r = o.extend(a.extend({}, a.pick(n, "inColumn"), {defaults: a.extend({}, n.prototype.defaults, o.prototype.defaults, {type: "pos"})})), t.exports.sel = s, t.exports.possel = r, t.exports.rowsel = o, t.exports.columnsel = n
}, {backbone: 4, underscore: 43}], 55: [function (e, t) {
    var n, i, r, o;
    r = e("./Selection"), o = e("underscore"), n = e("backbone").Collection, t.exports = i = n.extend({model: r.sel, initialize: function (e, t) {
        return this.g = t.g, this.listenTo(this.g, "residue:click", function (e) {
            return this._handleE(e.evt, new r.possel({xStart: e.rowPos, xEnd: e.rowPos, seqId: e.seqId}))
        }), this.listenTo(this.g, "row:click", function (e) {
            return this._handleE(e.evt, new r.rowsel({xStart: e.rowPos, xEnd: e.rowPos, seqId: e.seqId}))
        }), this.listenTo(this.g, "column:click", function (e) {
            return this._handleE(e.evt, new r.columnsel({xStart: e.rowPos, xEnd: e.rowPos + e.stepSize - 1}))
        })
    }, getSelForRow: function (e) {
        return this.filter(function (t) {
            return t.inRow(e)
        })
    }, getSelForColumns: function (e) {
        return this.filter(function (t) {
            return t.inColumn(e)
        })
    }, getBlocksForRow: function (e, t) {
        var n, i, r, o, s, a, u, l;
        for (r = this.filter(function (t) {
            return t.inRow(e)
        }), n = [], o = 0, s = r.length; s > o; o++) {
            if (i = r[o], "row" === i.attributes.type) {
                n = function () {
                    u = [];
                    for (var e = 0; t >= 0 ? t >= e : e >= t; t >= 0 ? e++ : e--)u.push(e);
                    return u
                }.apply(this);
                break
            }
            n = n.concat(function () {
                l = [];
                for (var e = a = i.attributes.xStart, t = i.attributes.xEnd; t >= a ? t >= e : e >= t; t >= a ? e++ : e--)l.push(e);
                return l
            }.apply(this))
        }
        return n
    }, getAllColumnBlocks: function () {
        var e, t, n, i, r, o, s;
        for (e = [], r = this.filter(function (e) {
            return"column" === e.get("type")
        }), n = 0, i = r.length; i > n; n++)t = r[n], e = e.concat(function () {
            s = [];
            for (var e = o = t.attributes.xStart, n = t.attributes.xEnd; n >= o ? n >= e : e >= n; n >= o ? e++ : e--)s.push(e);
            return s
        }.apply(this));
        return e
    }, invertRow: function (e) {
        var t, n, i, s, a, u;
        for (s = this.where({type: "row"}), s = o.map(s, function (e) {
            return e.attributes.seqId
        }), n = o.filter(e, function (e) {
            return s.indexOf(e) >= 0 ? !1 : !0
        }), i = [], a = 0, u = n.length; u > a; a++)t = n[a], i.push(new r.rowsel({seqId: t}));
        return console.log(i), this.reset(i)
    }, invertCol: function (e) {
        var t, n, i, s, a, u, l, c;
        if (s = this.where({type: "column"}), s = o.reduce(s, function (e, t) {
            var n, i;
            return e.concat(function () {
                i = [];
                for (var e = n = t.attributes.xStart, r = t.attributes.xEnd; r >= n ? r >= e : e >= r; r >= n ? e++ : e--)i.push(e);
                return i
            }.apply(this))
        }, []), n = o.filter(e, function (e) {
            return s.indexOf(e) >= 0 ? !1 : !0
        }), 0 !== n.length) {
            for (i = [], console.log(n), u = a = n[0], l = 0, c = n.length; c > l; l++)t = n[l], a + 1 === t ? a = t : (i.push(new r.columnsel({xStart: u, xEnd: a})), u = a = t);
            return u !== a && i.push(new r.columnsel({xStart: u, xEnd: n[n.length - 1]})), this.reset(i)
        }
    }, _handleE: function (e, t) {
        return e.ctrlKey || e.metaKey ? this.add(t) : this.reset([t])
    }})
}, {"./Selection": 54, backbone: 4, underscore: 43}], 56: [function (e, t) {
    var n, i;
    n = e("backbone").Model, t.exports = i = n.extend({defaults: {sequences: !0, markers: !0, metacell: !1, conserv: !0, overviewbox: !1, labels: !0, labelName: !0, labelId: !0}})
}, {backbone: 4}], 57: [function (e, t) {
    var n, i;
    n = e("backbone").Model, t.exports = i = n.extend({defaults: {columnWidth: 15, metaWidth: 100, labelWidth: 100, alignmentWidth: "auto", alignmentHeight: "auto", rowHeight: 15, textVisible: !0, labelLength: 20, labelFontsize: "10px", stepSize: 1, boxRectHeight: 5, boxRectWidth: 5, _alignmentScrollLeft: 0}, getAlignmentWidth: function (e) {
        return"auto" === this.get("alignmentWidth") ? this.get("columnWidth") * e : this.get("alignmentWidth")
    }})
}, {backbone: 4}], 58: [function (e, t) {
    t.exports.msa = e("./msa"), t.exports.model = e("./model"), t.exports.algo = e("./algo"), t.exports.menu = e("./menu"), t.exports.utils = e("./utils"), t.exports.selection = e("./g/selection/Selection"), t.exports.view = e("./bone/view"), t.exports.pluginator = e("./bone/pluginator"), t.exports._ = e("underscore"), t.exports.$ = e("jbone"), t.exports.version = "0.1.0"
}, {"./algo": 46, "./bone/pluginator": 48, "./bone/view": 49, "./g/selection/Selection": 54, "./menu": 60, "./model": 75, "./msa": 76, "./utils": 79, jbone: 42, underscore: 43}], 59: [function (e, t) {
    var n, i, r, o, s, a, u, l, c, h, d;
    d = e("../bone/pluginator"), a = e("./views/ImportMenu"), o = e("./views/FilterMenu"), c = e("./views/SelectionMenu"), h = e("./views/VisMenu"), n = e("./views/ColorMenu"), l = e("./views/OrderingMenu"), r = e("./views/ExtraMenu"), i = e("./views/ExportMenu"), s = e("./views/HelpMenu"), t.exports = u = d.extend({initialize: function (e) {
        return this.msa = e.msa, this.addView("10_import", new a({model: this.msa.seqs, g: this.msa.g})), this.addView("20_filter", new o({model: this.msa.seqs, g: this.msa.g})), this.addView("30_selection", new c({model: this.msa.seqs, g: this.msa.g})), this.addView("40_vis", new h({model: this.msa.seqs, g: this.msa.g})), this.addView("50_color", new n({model: this.msa.seqs, g: this.msa.g})), this.addView("60_ordering", new l({model: this.msa.seqs, g: this.msa.g})), this.addView("70_extra", new r({model: this.msa.seqs, g: this.msa.g})), this.addView("80_export", new i({model: this.msa.seqs, g: this.msa.g})), this.addView("90_help", new s)
    }, render: function () {
        return this.renderSubviews(), this.el.setAttribute("class", "biojs_msa_menubar"), this.el.appendChild(document.createElement("p"))
    }})
}, {"../bone/pluginator": 48, "./views/ColorMenu": 62, "./views/ExportMenu": 63, "./views/ExtraMenu": 64, "./views/FilterMenu": 65, "./views/HelpMenu": 66, "./views/ImportMenu": 67, "./views/OrderingMenu": 68, "./views/SelectionMenu": 69, "./views/VisMenu": 70}], 60: [function (e, t) {
    t.exports.defaultmenu = e("./defaultmenu"), t.exports.menubuilder = e("./menubuilder")
}, {"./defaultmenu": 59, "./menubuilder": 61}], 61: [function (e, t) {
    var n, i;
    n = e("../utils/bmath"), t.exports = i = function () {
        function e(e) {
            this.name = e, this._nodes = []
        }

        return e.prototype.addNode = function (e, t, n) {
            var i;
            return null != n && (i = n.style), this._nodes.push({label: e, callback: t, style: i})
        }, e.prototype.buildDOM = function () {
            return this._buildM({nodes: this._nodes, name: this.name})
        }, e.prototype._buildM = function (e) {
            var t, i, r, o, s, a, u, l, c, h, d, f, g;
            for (c = e.nodes, u = e.name, s = document.createElement("div"), s.className = "dropdown dropdown-tip", s.id = "adrop-" + n.uniqueId(), a = document.createElement("ul"), a.className = "dropdown-menu", d = 0, f = c.length; f > d; d++) {
                l = c[d], o = document.createElement("li"), o.textContent = l.label, g = l.style;
                for (r in g)h = g[r], o.style[r] = h;
                o.addEventListener("click", l.callback), a.appendChild(o)
            }
            return s.appendChild(a), i = document.createDocumentFragment(), t = document.createElement("a"), t.textContent = u, t.setAttribute("data-dropdown", "#" + s.id), t.className = "biojs_msa_menubar_alink", i.appendChild(s), i.appendChild(t), i
        }, e
    }()
}, {"../utils/bmath": 77}], 62: [function (e, t) {
    var n, i, r, o, s;
    o = e("../../bone/view"), i = e("../menubuilder"), s = e("underscore"), r = e("../../utils/dom"), t.exports = n = o.extend({initialize: function (e) {
        return this.g = e.g, this.el.style.display = "inline-block", this.listenTo(this.g.colorscheme, "change", function () {
            return this.render()
        })
    }, render: function () {
        var e, t, n, o, s, a;
        for (t = new i("Color scheme"), e = this.getColorschemes(), s = 0, a = e.length; a > s; s++)n = e[s], this.addScheme(t, n);
        return o = "Background", o = this.g.colorscheme.get("colorBackground") ? "Hide " + o : "Show " + o, t.addNode(o, function (e) {
            return function () {
                return e.g.colorscheme.set("colorBackground", !e.g.colorscheme.get("colorBackground"))
            }
        }(this)), this.grey(t), r.removeAllChilds(this.el), this.el.appendChild(t.buildDOM()), this
    }, addScheme: function (e, t) {
        var n, i;
        return i = {}, n = this.g.colorscheme.get("scheme"), n === t.id && (i.backgroundColor = "#77ED80"), e.addNode(t.name, function (e) {
            return function () {
                return e.g.colorscheme.set("scheme", t.id)
            }
        }(this), {style: i})
    }, getColorschemes: function () {
        var e;
        return e = [], e.push({name: "Zappo", id: "zappo"}), e.push({name: "Taylor", id: "taylor"}), e.push({name: "Hydrophobicity", id: "hydrophobicity"}), e.push({name: "PID", id: "pid"}), e.push({name: "No color", id: "foo"}), e
    }, grey: function (e) {
        return e.addNode("Grey", function (e) {
            return function () {
                return e.g.colorscheme.set("showLowerCase", !1), e.model.each(function (e) {
                    var t, n;
                    return n = e.get("seq"), t = [], s.each(n, function (e, n) {
                        return e === e.toLowerCase() ? t.push(n) : void 0
                    }), e.set("grey", t)
                })
            }
        }(this)), e.addNode("Grey by threshold", function (e) {
            return function () {
                var t, n, i, r, o, s, a;
                for (o = prompt("Enter threshold (in percent)", 20), o /= 100, r = e.model.getMaxLength(), t = e.g.columns.get("conserv"), n = [], i = s = 0, a = r - 1; a >= 0 ? a >= s : s >= a; i = a >= 0 ? ++s : --s)console.log(t[i]), t[i] < o && n.push(i);
                return e.model.each(function (e) {
                    return e.set("grey", n)
                })
            }
        }(this)), e.addNode("Grey selection", function (e) {
            return function () {
                var t;
                return t = e.model.getMaxLength(), e.model.each(function (n) {
                    var i;
                    return i = e.g.selcol.getBlocksForRow(n.get("id"), t), n.set("grey", i)
                })
            }
        }(this)), e.addNode("Reset grey", function (e) {
            return function () {
                return e.g.colorscheme.set("showLowerCase", !0), e.model.each(function (e) {
                    return e.set("grey", [])
                })
            }
        }(this))
    }})
}, {"../../bone/view": 49, "../../utils/dom": 78, "../menubuilder": 61, underscore: 43}], 63: [function (e, t) {
    var n, i, r, o, s, a;
    s = e("../../bone/view"), r = e("../menubuilder"), o = e("../../../external/saver"), i = e("biojs-io-fasta").writer, a = e("underscore"), t.exports = n = s.extend({initialize: function (e) {
        return this.g = e.g
    }, render: function () {
        var t;
        return t = new r("Export"), t.addNode("Export sequences", function (e) {
            return function () {
                var t, n;
                return n = i["export"](e.model.toJSON()), t = new Blob([n], {type: "text/plain"}), o(t, "all.fasta")
            }
        }(this)), t.addNode("Export selection", function (e) {
            return function () {
                var t, n, r, s, u, l;
                if (r = e.g.selcol.pluck("seqId"), null != r)for (r = e.model.filter(function (e) {
                    return a.contains(r, e.get("id"))
                }), n = u = 0, l = r.length - 1; l >= u; n = u += 1)r[n] = r[n].toJSON(); else r = e.model.toJSON(), console.log("no selection found");
                return s = i["export"](r), t = new Blob([s], {type: "text/plain"}), o(t, "selection.fasta")
            }
        }(this)), t.addNode("Export image (broken)", function (t) {
            return function () {
                return console.log("trying to render"), e(["html2canvas", "saveAs"], function (e, n) {
                    return e(t.msa.container, {onrendered: function (e) {
                        return e.toBlob(function (e) {
                            return n(e, "biojs-msa.png")
                        }, "image/png")
                    }})
                }, function () {
                    return console.log("couldn't load HTML2canvas")
                })
            }
        }(this)), this.el = t.buildDOM(), this
    }})
}, {"../../../external/saver": 1, "../../bone/view": 49, "../menubuilder": 61, "biojs-io-fasta": 21, underscore: 43}], 64: [function (e, t) {
    var n, i, r, o, s;
    i = e("../menubuilder"), o = e("../../algo/ConsensusCalc"), r = e("../../model/Sequence"), s = e("../../bone/view"), t.exports = n = s.extend({initialize: function (e) {
        return this.g = e.g
    }, render: function () {
        var e;
        return e = new i("Extras"), e.addNode("Add consensus seq", function (e) {
            return function () {
                var t, n;
                return t = o(e.model), console.log(t), n = new r({seq: t, id: "0c", name: "consenus"}), e.model.add(n), e.model.comparator = function (e) {
                    return e.get("id")
                }, e.model.sort()
            }
        }(this)), e.addNode("Increase font size", function (e) {
            return function () {
                return e.g.zoomer.set("columnWidth", e.g.zoomer.get("columnWidth") + 2), e.g.zoomer.set("labelWidth", e.g.zoomer.get("columnWidth") + 5), e.g.zoomer.set("rowHeight", e.g.zoomer.get("rowHeight") + 2), e.g.zoomer.set("labelFontSize", e.g.zoomer.get("labelFontSize") + 2)
            }
        }(this)), e.addNode("Decrease font size", function (e) {
            return function () {
                return e.g.zoomer.set("columnWidth", e.g.zoomer.get("columnWidth") - 2), e.g.zoomer.set("rowHeight", e.g.zoomer.get("rowHeight") - 2), e.g.zoomer.set("labelFontSize", e.g.zoomer.get("labelFontSize") - 2), e.g.zoomer.get("columnWidth") < 8 ? e.g.zoomer.set("textVisible", !1) : void 0
            }
        }(this)), e.addNode("Bar chart exp scaling", function (e) {
            return function () {
                return e.g.columns.set("scaling", "exp")
            }
        }(this)), e.addNode("Bar chart linear scaling", function (e) {
            return function () {
                return e.g.columns.set("scaling", "lin")
            }
        }(this)), e.addNode("Bar chart log scaling", function (e) {
            return function () {
                return e.g.columns.set("scaling", "log")
            }
        }(this)), e.addNode("Minimized width", function (e) {
            return function () {
                return e.g.zoomer.set("alignmentWidth", 600)
            }
        }(this)), e.addNode("Minimized height", function (e) {
            return function () {
                return e.g.zoomer.set("alignmentHeight", 120)
            }
        }(this)), this.el = e.buildDOM(), this
    }})
}, {"../../algo/ConsensusCalc": 44, "../../bone/view": 49, "../../model/Sequence": 74, "../menubuilder": 61}], 65: [function (e, t) {
    var n, i, r, o;
    r = e("../../bone/view"), i = e("../menubuilder"), o = e("underscore"), t.exports = n = r.extend({initialize: function (e) {
        return this.g = e.g
    }, render: function () {
        var e;
        return e = new i("Filter"), e.addNode("Hide columns by threshold", function (e) {
            return function () {
                var t, n, i, r, o, s, a;
                for (o = prompt("Enter threshold (in percent)", 20), o /= 100, r = e.model.getMaxLength(), n = [], t = e.g.columns.get("conserv"), i = s = 0, a = r - 1; a >= 0 ? a >= s : s >= a; i = a >= 0 ? ++s : --s)t[i] < o && n.push(i);
                return e.g.columns.set("hidden", n)
            }
        }(this)), e.addNode("Hide columns by selection", function (e) {
            return function () {
                var t, n;
                return n = e.g.columns.get("hidden"), t = n.concat(e.g.selcol.getAllColumnBlocks(e.model.getMaxLength())), e.g.selcol.reset([]), e.g.columns.set("hidden", t)
            }
        }(this)), e.addNode("Hide columns by gaps", function (e) {
            return function () {
                var t, n, i, r, o, s, a, u, l;
                for (s = prompt("Enter threshold (in percent)", 20), s /= 100, o = e.model.getMaxLength(), i = [], r = u = 0, l = o - 1; l >= 0 ? l >= u : u >= l; r = l >= 0 ? ++u : --u)n = 0, a = 0, e.model.each(function (e) {
                    return"-" === e.get("seq")[r] && n++, a++
                }), t = n / a, t > s && i.push(r);
                return e.g.columns.set("hidden", i)
            }
        }(this)), e.addNode("Hide seqs by identity", function (e) {
            return function () {
                var t;
                return t = prompt("Enter threshold (in percent)", 20), t /= 100, e.model.each(function (e) {
                    return e.get("identity") < t ? e.set("hidden", !0) : void 0
                })
            }
        }(this)), e.addNode("Hide seqs by selection", function (e) {
            return function () {
                var t, n;
                return t = e.g.selcol.where({type: "row"}), n = o.map(t, function (e) {
                    return e.get("seqId")
                }), e.model.each(function (e) {
                    return n.indexOf(e.get("id")) >= 0 ? e.set("hidden", !0) : void 0
                })
            }
        }(this)), e.addNode("Hide seqs by gaps", function (e) {
            return function () {
                var t;
                return t = prompt("Enter threshold (in percent)", 40), e.model.each(function (e) {
                    var n, i;
                    return i = e.get("seq"), n = o.reduce(i, function (e, t) {
                        return"-" === t && e++, e
                    }, 0), console.log(n), n > t ? e.set("hidden", !0) : void 0
                })
            }
        }(this)), e.addNode("Reset", function (e) {
            return function () {
                return e.g.columns.set("hidden", []), e.model.each(function (e) {
                    return e.get("hidden") ? e.set("hidden", !1) : void 0
                })
            }
        }(this)), this.el = e.buildDOM(), this
    }})
}, {"../../bone/view": 49, "../menubuilder": 61, underscore: 43}], 66: [function (e, t) {
    var n, i, r;
    r = e("../../bone/view"), i = e("../menubuilder"), t.exports = n = r.extend({render: function () {
        var e;
        return e = new i("Help"), e.addNode("About the project", function () {
            return function () {
                return window.open("https://github.com/greenify/biojs-vis-msa")
            }
        }(this)), e.addNode("Report issues", function () {
            return function () {
                return window.open("https://github.com/greenify/biojs-vis-msa/issues")
            }
        }(this)), e.addNode("User manual", function () {
            return function () {
                return window.open("https://github.com/greenify/biojs-vis-msa/wiki")
            }
        }(this)), this.el = e.buildDOM(), this
    }})
}, {"../../bone/view": 49, "../menubuilder": 61}], 67: [function (e, t) {
    var n, i, r, o, s;
    s = e("../../bone/view"), s = e("../../bone/view"), n = e("biojs-io-clustal"), i = e("biojs-io-fasta").parse, o = e("../menubuilder"), t.exports = r = s.extend({initialize: function (e) {
        return this.g = e.g
    }, render: function () {
        var e, t;
        return t = new o("Import"), e = function (e) {
            return document.URL.indexOf("localhost") >= 0 && "/" === e[0] ? e : e = e.indexOf("www") >= 0 ? e.replace("www.", "www.corsproxy.com/") : e.indexOf("http://") >= 0 ? e.replace("://", "://www.corsproxy.com/") : "http://www.corsproxy.com/" + e
        }, t.addNode("FASTA", function (t) {
            return function () {
                var n;
                return n = prompt("URL", "/test/dummy/samples/p53.clustalo.fasta"), n = e(n), i.read(n, function (e) {
                    var n;
                    return n = t.g.zoomer.toJSON(), n.labelWidth = 200, n.stepSize = 2, n.boxRectHeight = 2, n.boxRectWidth = 2, t.model.reset([]), t.g.zoomer.set(n), t.model.reset(e), t.g.columns.calcConservation(t.model)
                })
            }
        }(this)), t.addNode("CLUSTAL", function (t) {
            return function () {
                var i;
                return i = prompt("URL", "/test/dummy/samples/p53.clustalo.clustal"), i = e(i), n.read(i, function (e) {
                    var n;
                    return n = t.g.zoomer.toJSON(), n.stepSize = 2, n.labelWidth = 200, n.boxRectHeight = 2, n.boxRectWidth = 2, t.model.reset([]), t.g.zoomer.set(n), t.model.reset(e), t.g.columns.calcConservation(t.model)
                })
            }
        }(this)), t.addNode("add your own Parser", function () {
            return function () {
                return window.open("https://github.com/biojs/biojs2")
            }
        }(this)), this.el = t.buildDOM(), this
    }})
}, {"../../bone/view": 49, "../menubuilder": 61, "biojs-io-clustal": 8, "biojs-io-fasta": 21}], 68: [function (e, t) {
    var n, i, r, o;
    o = e("../../bone/view"), n = e("../menubuilder"), r = e("../../utils/dom"), t.exports = i = o.extend({initialize: function () {
        return this.order = "ID", this.el.style.display = "inline-block"
    }, setOrder: function (e) {
        return this.order = e, this.render()
    }, render: function () {
        var e, t, i, o, s, a;
        for (o = new n("Ordering"), e = this.getComparators(), s = 0, a = e.length; a > s; s++)i = e[s], this._addNode(o, i);
        return t = o.buildDOM(), r.removeAllChilds(this.el), this.el.appendChild(t), this
    }, _addNode: function (e, t) {
        var n, i;
        return i = t.text, n = {}, i === this.order && (n.backgroundColor = "#77ED80"), e.addNode(i, function (e) {
            return function () {
                return e.model.comparator = t.comparator, e.model.sort(), e.setOrder(t.text)
            }
        }(this), {style: n})
    }, getComparators: function () {
        var e;
        return e = [], e.push({text: "ID", comparator: "id"}), e.push({text: "ID Desc", comparator: function (e, t) {
            return-e.get("id").localeCompare(t.get("id"))
        }}), e.push({text: "Label", comparator: "name"}), e.push({text: "Label Desc", comparator: function (e, t) {
            return-e.get("name").localeCompare(t.get("name"))
        }}), e.push({text: "Seq", comparator: "seq"}), e.push({text: "Seq Desc", comparator: function (e, t) {
            return-e.get("seq").localeCompare(t.get("seq"))
        }}), e.push({text: "Identity", comparator: "identity"}), e.push({text: "Identity Desc", comparator: function (e) {
            return-e.get("identity")
        }}), e
    }})
}, {"../../bone/view": 49, "../../utils/dom": 78, "../menubuilder": 61}], 69: [function (e, t) {
    var n, i, r, o;
    o = e("../../bone/view"), r = e("../../g/selection/Selection"), n = e("../menubuilder"), t.exports = i = o.extend({initialize: function (e) {
        return this.g = e.g
    }, render: function () {
        var e;
        return e = new n("Selection"), e.addNode("Find all (supports RegEx)", function (e) {
            return function () {
                var t, n, i;
                return n = prompt("your search", "D"), n = new RegExp(n, "gi"), i = e.g.selcol, t = [], e.model.each(function (e) {
                    var i, o, s, a, u;
                    for (a = e.get("seq"), u = []; s = n.exec(a);)o = s.index, i = {xStart: o, xEnd: o + s[0].length - 1, seqId: e.get("id")}, u.push(t.push(new r.possel(i)));
                    return u
                }), i.reset(t)
            }
        }(this)), e.addNode("Select all", function (e) {
            return function () {
                var t, n, i, o, s;
                for (i = e.model.pluck("id"), n = [], o = 0, s = i.length; s > o; o++)t = i[o], n.push(new r.rowsel({seqId: t}));
                return e.g.selcol.reset(n)
            }
        }(this)), e.addNode("Invert columns", function (e) {
            return function () {
                var t;
                return e.g.selcol.invertCol(function () {
                    t = [];
                    for (var n = 0, i = e.model.getMaxLength(); i >= 0 ? i >= n : n >= i; i >= 0 ? n++ : n--)t.push(n);
                    return t
                }.apply(this))
            }
        }(this)), e.addNode("Invert rows", function (e) {
            return function () {
                return e.g.selcol.invertRow(e.model.pluck("id"))
            }
        }(this)), e.addNode("Reset", function (e) {
            return function () {
                return e.g.selcol.reset()
            }
        }(this)), this.el = e.buildDOM(), this
    }})
}, {"../../bone/view": 49, "../../g/selection/Selection": 54, "../menubuilder": 61}], 70: [function (e, t) {
    var n, i, r, o;
    o = e("../../bone/view"), i = e("../menubuilder"), r = e("../../utils/dom"), t.exports = n = o.extend({initialize: function (e) {
        return this.g = e.g, this.el.style.display = "inline-block", this.listenTo(this.g.vis, "change", this.render)
    }, render: function () {
        var e, t, n, o, s;
        for (e = new i("Vis. elements"), n = this.getVisElements(), o = 0, s = n.length; s > o; o++)t = n[o], this._addVisEl(e, t);
        return e.addNode("Reset", function (e) {
            return function () {
                return e.g.vis.set("labels", !0), e.g.vis.set("sequences", !0), e.g.vis.set("metacell", !0), e.g.vis.set("conserv", !0)
            }
        }(this)), e.addNode("Toggle mouseover events", function (e) {
            return function () {
                return e.g.config.set("registerMouseEvents", !e.g.config.get("registerMouseEvents"))
            }
        }(this)), r.removeAllChilds(this.el), this.el.appendChild(e.buildDOM()), this
    }, _addVisEl: function (e, t) {
        var n, i;
        return i = {}, this.g.vis.get(t.id) ? (n = "Hide ", i.color = "red") : (n = "Show ", i.color = "green"), e.addNode(n + t.name, function (e) {
            return function () {
                return e.g.vis.set(t.id, !e.g.vis.get(t.id))
            }
        }(this), {style: i})
    }, getVisElements: function () {
        var e;
        return e = [], e.push({name: "Markers", id: "markers"}), e.push({name: "Labels", id: "labels"}), e.push({name: "Sequences", id: "sequences"}), e.push({name: "Meta info", id: "metacell"}), e.push({name: "conserv", id: "conserv"}), e
    }})
}, {"../../bone/view": 49, "../../utils/dom": 78, "../menubuilder": 61}], 71: [function (e, t) {
    var n, i;
    n = e("./Feature"), i = e("backbone").Model, t.exports = n = i.extend({defaults: {xStart: -1, xEnd: -1, height: -1, text: "", fillColor: "red", fillOpacity: .5, type: "rectangle", borderSize: 1, borderColor: "black", borderOpacity: .5, validate: !0}, validate: function () {
        return isNaN(this.attributes.xStart || isNaN(this.attributes.xEnd)) ? "features need integer start and end." : void 0
    }, contains: function (e) {
        return this.attributes.xStart <= e && e <= this.attributes.xEnd
    }})
}, {"./Feature": 71, backbone: 4}], 72: [function (e, t) {
    var n, i, r, o;
    i = e("./Feature"), n = e("backbone").Collection, o = e("underscore"), t.exports = r = n.extend({model: i, startOn: function (e) {
        return this.where({xStart: e})
    }, contains: function (e) {
        return this.reduce(function (t, n) {
            return n || t.contains(e)
        }, !1)
    }, getMinRows: function () {
        var e, t, n;
        return e = this.max(function (e) {
            return e.get("xEnd")
        }), t = function () {
            var t, i;
            for (i = [], n = t = 1; e >= 1 ? e >= t : t >= e; n = e >= 1 ? ++t : --t)i.push(0);
            return i
        }(), this.each(function (e) {
            var i, r, o, s;
            for (s = [], n = i = r = e.get("xStart"), o = feature.get("xEnd"); o >= i; n = i += 1)s.push(t[n]++);
            return s
        }), o.max(t)
    }})
}, {"./Feature": 71, backbone: 4, underscore: 43}], 73: [function (e, t) {
    var n, i, r;
    r = e("./Sequence"), n = e("backbone").Collection, t.exports = i = n.extend({model: r, getMaxLength: function () {
        return 0 === this.models.length ? 0 : this.max(function (e) {
            return e.get("seq").length
        }).get("seq").length
    }, prev: function (e, t) {
        var n;
        return n = this.indexOf(e) - 1, 0 > n && t && (n = this.length - 1), this.at(n)
    }, next: function (e, t) {
        var n;
        return n = this.indexOf(e) + 1, n === this.length && t && (n = 0), this.at(n)
    }})
}, {"./Sequence": 74, backbone: 4}], 74: [function (e, t) {
    var n, i, r;
    i = e("backbone").Model, n = e("./FeatureCol"), t.exports = r = i.extend({defaults: {name: "", id: "", seq: ""}, initialize: function () {
        return this.set("grey", []), this.set("features", new n)
    }})
}, {"./FeatureCol": 72, backbone: 4}], 75: [function (e, t) {
    t.exports.seq = e("./Sequence"), t.exports.seqcol = e("./SeqCollection"), t.exports.feature = e("./Feature"), t.exports.featurecol = e("./FeatureCol")
}, {"./Feature": 71, "./FeatureCol": 72, "./SeqCollection": 73, "./Sequence": 74}], 76: [function (e, t) {
    var n, i, r, o, s, a, u, l, c, h, d, f;
    l = e("./model/SeqCollection"), n = e("./g/colorator"), o = e("./g/consensus"), i = e("./g/columns"), r = e("./g/config"), u = e("./g/selection/SelectionCol"), h = e("./g/visibility"), d = e("./g/zoomer"), f = e("./bone/pluginator"), s = e("biojs-events"), c = e("./views/Stage"), a = f.extend({events: {dblclick: "cleanup"}, initialize: function (e) {
        return null == e.columns && (e.columns = {}), null == e.conf && (e.conf = {}), null == e.vis && (e.vis = {}), null == e.zoomer && (e.zoomer = {}), this.g = s.mixin({}), this.seqs = new l(e.seqs), this.g.config = new r(e.conf), this.g.consensus = new o, this.g.columns = new i(e.columns), this.g.colorscheme = new n, this.g.selcol = new u([], {g: this.g}), this.g.vis = new h(e.vis), this.g.zoomer = new d(e.zoomer), this.addView("stage", new c({model: this.seqs, g: this.g})), this.el.setAttribute("class", "biojs_msa_div")
    }, render: function () {
        return this.trigger("hello"), this.renderSubviews(), this
    }}), t.exports = a
}, {"./bone/pluginator": 48, "./g/colorator": 50, "./g/columns": 51, "./g/config": 52, "./g/consensus": 53, "./g/selection/SelectionCol": 55, "./g/visibility": 56, "./g/zoomer": 57, "./model/SeqCollection": 73, "./views/Stage": 87, "biojs-events": 5}], 77: [function (e, t) {
    var n;
    t.exports = n = function () {
        function e() {
        }

        return e.randomInt = function (e, t) {
            var n, i;
            return null == t && (n = [0, e], e = n[0], t = n[1]), e > t && (i = [t, e], e = i[0], t = i[1]), Math.floor(Math.random() * (t - e + 1) + e)
        }, e.uniqueId = function (e) {
            var t;
            for (null == e && (e = 8), t = ""; t.length < e;)t += Math.random().toString(36).substr(2);
            return t.substr(0, e)
        }, e.getRandomInt = function (e, t) {
            return Math.floor(Math.random() * (t - e + 1)) + e
        }, e
    }()
}, {}], 78: [function (e, t) {
    var n;
    n = {}, n.removeToInsertLater = function (e) {
        var t, n;
        return n = e.parentNode, t = e.nextSibling, n.removeChild(e), function () {
            t ? n.insertBefore(e, t) : n.appendChild(e)
        }
    }, n.removeAllChilds = function (e) {
        var t;
        for (t = 0; e.firstChild;)t++, e.removeChild(e.firstChild)
    }, t.exports = n
}, {}], 79: [function (e, t) {
    t.exports.bmath = e("./bmath"), t.exports.seqgen = e("./seqgen")
}, {"./bmath": 77, "./seqgen": 81}], 80: [function (e, t) {
    var n;
    t.exports = n = {getMouseCoords: function (e) {
        var t, n, i, r;
        return t = e.offsetX, n = e.offsetY, null == t && (r = e.target || e.srcElement, i = r.getBoundingClientRect(), null == t && (t = e.clientX - i.left, n = e.clientY - i.top), null == t) ? (console.log(e), void console.log("no mouse event defined. your browser sucks")) : [t, n]
    }}
}, {}], 81: [function (e, t) {
    var n, i, r;
    i = e("biojs-model").seq, n = e("./bmath"), r = t.exports = {_generateSequence: function (e) {
        var t, n, i, r, o;
        for (i = "", n = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", t = r = 0, o = e - 1; o >= r; t = r += 1)i += n.charAt(Math.floor(Math.random() * n.length));
        return i
    }, getDummySequences: function (e, t) {
        var o, s, a, u;
        for (s = [], null == e && (e = n.getRandomInt(3, 5)), null == t && (t = n.getRandomInt(50, 200)), o = a = 0, u = e - 1; u >= a; o = a += 1)s.push(new i(r._generateSequence(t), "seq" + o, "r" + o));
        return s
    }, splitNChars: function (e, t) {
        var n, i;
        for (i = [], n = 0; n < e.length;)i.push(e.substr(n, t)), n += t;
        return i
    }}
}, {"./bmath": 77, "biojs-model": 36}], 82: [function (e, t) {
    var n, i, r, o, s, a;
    a = "http://www.w3.org/2000/svg", s = function (e, t) {
        var n, i;
        for (n in t)i = t[n], e.setAttributeNS(null, n, i);
        return e
    }, n = function (e) {
        var t;
        return t = document.createElementNS(a, "svg"), t.setAttribute("width", e.width), t.setAttribute("height", e.height), t
    }, o = function (e) {
        var t;
        return t = document.createElementNS(a, "rect"), s(t, e)
    }, i = function (e) {
        var t;
        return t = document.createElementNS(a, "line"), s(t, e)
    }, r = function (e) {
        var t;
        return t = document.createElementNS(a, "polygon"), s(t, e)
    }, t.exports.rect = o, t.exports.line = i, t.exports.polygon = r, t.exports.base = n
}, {}], 83: [function (e, t) {
    var n, i, r;
    r = e("../bone/pluginator"), i = e("./SeqBlock"), n = e("./labels/LabelBlock"), t.exports = r.extend({initialize: function (e) {
        var t, r;
        return this.g = e.g, t = new n({model: this.model, g: this.g}), t.ordering = -1, this.addView("labelblock", t), this.g.vis.get("sequences") && (r = new i({model: this.model, g: this.g}), r.ordering = 0, this.addView("seqblock", r)), this.listenTo(this.g.zoomer, "change:alignmentHeight", this.adjustHeight), this.listenTo(this.g.columns, "change:hidden", this.adjustHeight)
    }, render: function () {
        return this.renderSubviews(), this.el.className = "biojs_msa_albody", this.el.style.overflowY = "auto", this.el.style.overflowX = "visible", this.el.style.whiteSpace = "nowrap", this.adjustHeight(), this
    }, adjustHeight: function () {
        return this.el.style.height = "auto" === this.g.zoomer.get("alignmentHeight") ? this.g.zoomer.get("rowHeight") * this.model.length + 5 : this.g.zoomer.get("alignmentHeight"), this.el.style.width = this.getWidth() + 15
    }, getWidth: function () {
        var e;
        return e = 0, this.g.vis.get("labels") && (e += this.g.zoomer.get("labelWidth")), this.g.vis.get("metacell") && (e += this.g.zoomer.get("metaWidth")), this.g.vis.get("sequences") && (e += this.g.zoomer.get("alignmentWidth")), e
    }})
}, {"../bone/pluginator": 48, "./SeqBlock": 85, "./labels/LabelBlock": 94}], 84: [function (e, t) {
    var n, i, r, o, s, a, u;
    u = e("../bone/view"), s = e("../utils/mouse"), a = e("../g/selection/Selection"), r = e("./color/taylor"), o = e("./color/zappo"), n = e("./color/hydrophobicity"), t.exports = i = u.extend({className: "biojs_msa_overviewbox", tagName: "canvas", initialize: function (e) {
        return this.g = e.g, this.listenTo(this.g.zoomer, "change:boxRectWidth change:boxRectHeight", this.render), this.listenTo(this.g.selcol, "add reset change", this.render), this.listenTo(this.g.columns, "change:hidden", this.render), this.listenTo(this.g.colorscheme, "change:showLowerCase", this.render), this._setColorScheme(), this.listenTo(this.g.colorscheme, "change:scheme", function () {
            return this._setColorScheme(), this.render()
        }), this.dragStart = []
    }, events: {click: "_onclick", mousedown: "_onmousedown", mouseup: "_onmouseup", mouseout: "_onmouseout", mousemove: "_onmousemove"}, render: function () {
        var e, t, n, i, r, o, s, a, u, l, c, h, d, f, g;
        for (this._createCanvas(), this.el.textContent = "overview", this.ctx.fillStyle = "#999999", this.ctx.fillRect(0, 0, this.el.width, this.el.height), s = this.g.zoomer.get("boxRectWidth"), o = this.g.zoomer.get("boxRectHeight"), n = this.g.columns.get("hidden"), u = this.g.colorscheme.get("showLowerCase"), c = 0, i = h = 0, f = this.model.length - 1; f >= h; i = h += 1) {
            for (a = this.model.at(i).get("seq"), l = 0, r = d = 0, g = a.length - 1; g >= d; r = d += 1)e = a[r], u && (e = e.toUpperCase()), t = this.color[e], n.indexOf(r) >= 0 ? this.ctx.globalAlpha = .3 : 1 !== this.ctx.globalAlpha && (this.ctx.globalAlpha = 1), null != t && (this.ctx.fillStyle = "#" + t, this.ctx.fillRect(l, c, s, o)), l += s;
            c += o
        }
        return this._drawSelection()
    }, _drawSelection: function () {
        var e, t, n, i, r, o, s, a, u;
        if (!(this.dragStart.length > 0) || this.prolongSelection) {
            for (r = this.g.zoomer.get("boxRectWidth"), i = this.g.zoomer.get("boxRectHeight"), t = i * this.model.length, this.ctx.fillStyle = "#ffff00", this.ctx.globalAlpha = .5, e = a = 0, u = this.g.selcol.length - 1; u >= a; e = a += 1)o = this.g.selcol.at(e), "column" === o.get("type") ? this.ctx.fillRect(r * o.get("xStart"), 0, r * (o.get("xEnd") - o.get("xStart") + 1), t) : "row" === o.get("type") ? (s = this.model.filter(function (e) {
                return e.get("id") === o.get("seqId")
            })[0], n = this.model.indexOf(s), this.ctx.fillRect(0, i * n, r * s.get("seq").length, i)) : "pos" === o.get("type") && (s = this.model.filter(function (e) {
                return e.get("id") === o.get("seqId")
            })[0], n = this.model.indexOf(s), this.ctx.fillRect(r * o.get("xStart"), i * n, r * (o.get("xEnd") - o.get("xStart") + 1), i));
            return this.ctx.globalAlpha = 1
        }
    }, _onclick: function (e) {
        return this.g.trigger("meta:click", {seqId: this.model.get("id", {evt: e})})
    }, _onmousemove: function (e) {
        return 0 !== this.dragStart.length ? (this.dragEnd = s.getMouseCoords(e), this.render(), this.ctx.fillStyle = "#ffff00", this.ctx.globalAlpha = .5, this.ctx.fillRect(this.dragStart[0], this.dragStart[1], this.dragEnd[0] - this.dragStart[0], this.dragEnd[1] - this.dragStart[1])) : void 0
    }, _onmousedown: function (e) {
        var t;
        return this.dragStart = t = s.getMouseCoords(e), this.prolongSelection = e.ctrlKey || e.metaKey ? !0 : !1, this.dragStart
    }, _endSelection: function (e) {
        var t, n, i, r, o, s, u, l, c, h, d;
        if (0 !== this.dragStart.length) {
            for (s = this.g.zoomer.get("boxRectWidth"), o = this.g.zoomer.get("boxRectHeight"), t = new Array(4), t[0] = Math.min(this.dragStart[0], e[0]), t[1] = Math.min(this.dragStart[1], e[1]), t[2] = Math.max(this.dragStart[0], e[0]), t[3] = Math.max(this.dragStart[1], e[1]), i = l = 0; 3 >= l; i = ++l)t[i] = Math.floor(i % 2 === 0 ? t[i] / s : t[i] / o), 1 >= i && (t[i] = Math.max(0, t[i]));
            for (t[2] = Math.min(this.model.getMaxLength(), t[2]), t[3] = Math.min(this.model.length - 1, t[3]), u = [], r = c = h = t[1], d = t[3]; d >= c; r = c += 1)n = {seqId: this.model.at(r).get("id"), xStart: t[0], xEnd: t[2]}, u.push(new a.possel(n));
            return this.dragStart = [], this.prolongSelection ? this.g.selcol.add(u) : this.g.selcol.reset(u)
        }
    }, _onmouseup: function (e) {
        return this._endSelection(s.getMouseCoords(e))
    }, _onmouseout: function (e) {
        return this._endSelection(s.getMouseCoords(e))
    }, _setColorScheme: function () {
        var e;
        return e = this.g.colorscheme.get("scheme"), "taylor" === e ? this.color = r : "zappo" === e ? this.color = o : "hydrophobicity" === e ? this.color = n : console.log("warning. no color scheme found.")
    }, _createCanvas: function () {
        var e, t;
        return t = this.g.zoomer.get("boxRectWidth"), e = this.g.zoomer.get("boxRectHeight"), this.el.height = this.model.length * e, this.el.width = this.model.getMaxLength() * t, this.ctx = this.el.getContext("2d"), this.el.style.overflow = "scroll", this.el.style.cursor = "crosshair"
    }})
}, {"../bone/view": 49, "../g/selection/Selection": 54, "../utils/mouse": 80, "./color/hydrophobicity": 88, "./color/taylor": 89, "./color/zappo": 90}], 85: [function (e, t) {
    var n, i;
    n = e("./SeqView"), i = e("../bone/pluginator"), t.exports = i.extend({initialize: function (e) {
        return this.g = e.g, this.listenTo(this.g.zoomer, "change:_alignmentScrollLeft", this._adjustScrollingLeft), this.listenTo(this.g.columns, "change:hidden", this._adjustWidth), this.listenTo(this.g.zoomer, "change:alignmentWidth", this._adjustWidth), this.draw()
    }, draw: function () {
        var e, t, i, r, o;
        for (this.removeViews(), o = [], e = i = 0, r = this.model.length - 1; r >= i; e = i += 1)this.model.at(e).get("hidden") || (t = new n({model: this.model.at(e), g: this.g}), t.ordering = e, o.push(this.addView("row_" + e, t)));
        return o
    }, render: function () {
        return this.renderSubviews(), this.el.className = "biojs_msa_seq_st_block", this.el.style.display = "inline-block", this.el.style.overflowX = "hidden", this.el.style.overflowY = "hidden", this._adjustWidth(), this
    }, _adjustScrollingLeft: function () {
        return this.el.scrollLeft = this.g.zoomer.get("_alignmentScrollLeft")
    }, _getLabelWidth: function () {
        var e;
        return e = 0, this.g.vis.get("labels") && (e += this.g.zoomer.get("labelWidth")), this.g.vis.get("metacell") && (e += this.g.zoomer.get("metaWidth")), e
    }, _adjustWidth: function () {
        var e, t;
        return t = null != this.el.parentNode ? this.el.parentNode.offsetWidth : document.body.clientWidth, this.maxWidth = t - this._getLabelWidth() - 35, this.el.style.width = this.g.zoomer.get("alignmentWidth"), e = this.g.zoomer.getAlignmentWidth(this.model.getMaxLength() - this.g.columns.get("hidden").length), e > this.maxWidth && this.g.zoomer.set("alignmentWidth", this.maxWidth), this.el.style.width = Math.min(e, this.maxWidth)
    }})
}, {"../bone/pluginator": 48, "./SeqView": 86}], 86: [function (e, t) {
    var n, i, r, o, s, a;
    s = e("../bone/view"), i = e("../utils/dom"), o = e("../utils/svg"), a = e("underscore"), r = e("jbone"), n = s.extend({initialize: function (e) {
        return this.g = e.g, this._build(), this.listenTo(this.model, "change:grey", this._build), this.listenTo(this.g.columns, "change:hidden", this._build), this.listenTo(this.model, "change:features", this._build), this.listenTo(this.g.selcol, "add", function () {
            return 0 !== this.g.selcol.getSelForRow(this.model.get("id")).length ? this._appendSelection() : void 0
        }), this.listenTo(this.g.selcol, "reset", this._build), this.listenTo(this.g.zoomer, "change", this._build), this.listenTo(this.g.colorscheme, "change", function () {
            return this._build(), this.render()
        }), this.manageEvents()
    }, manageEvents: function () {
        var e;
        return e = {}, e.click = "_onclick", this.g.config.get("registerMouseEvents") && (e.mousein = "_onmousein", e.mouseout = "_onmouseout"), this.delegateEvents(e), this.listenTo(this.g.config, "change:registerMouseEvents", this.manageEvents)
    }, _build: function () {
        var e, t, n, r, o, s, a, u, l, c, h, d, f, g, p;
        for (i.removeAllChilds(this.el), u = this.model.get("seq"), o = this.g.columns.get("hidden"), h = this.g.zoomer.get("textVisible"), r = this.model.get("features"), e = this.g.zoomer.get("rowHeight"), t = this.g.zoomer.get("columnWidth"), s = 0, a = d = 0, p = u.length - 1; p >= d; a = d += 1)if (o.indexOf(a) < 0) {
            if (l = document.createElement("span"), l.rowPos = a, h ? l.textContent = u[a] : l.style.height = e, c = r.startOn(a), c.length > 0)for (f = 0, g = c.length; g > f; f++)n = c[f], l.appendChild(this.appendFeature(n));
            this._drawResidue(l, u[a], a), l.style.width = t, l.style.position = "relative", this.el.appendChild(l)
        }
        return this._appendSelection()
    }, _appendSelection: function () {
        var e, t, n, i, r, o, s, a, u, l, c, h, d, f, g, p, m;
        if (l = this.model.get("seq"), u = this._getSelection(this.model), n = this.g.columns.get("hidden"), f = this._getPrevNextSelection(), s = f[0], o = f[1], t = this.el.children, 0 !== u.length) {
            for (i = 0, m = [], a = c = 0, g = l.length - 1; g >= c; a = c += 1)if (n.indexOf(a) >= 0)m.push(i++); else if (r = a - i, null != t[r]) {
                if (t[r].children.length > 0)for (p = t[r].children, h = 0, d = p.length; d > h; h++)e = p[h], "selection" === e.type && t[r].removeChild(e);
                m.push(u.indexOf(a) >= 0 && (0 === r || u.indexOf(a - 1) < 0) ? t[r].appendChild(this._renderSelection(a, u, s, o)) : void 0)
            } else m.push(void 0);
            return m
        }
    }, render: function () {
        return this.el.style.height = this.g.zoomer.get("rowHeight"), this.el.className = "biojs_msa_seqblock", this.el.className += this.g.colorscheme.get("colorBackground") ? " biojs-msa-schemes-" + this.g.colorscheme.get("scheme") : " biojs-msa-schemes-" + this.g.colorscheme.get("scheme") + "-bl"
    }, _onclick: function (e) {
        var t;
        return t = this.model.get("id"), this.g.trigger("residue:click", {seqId: t, rowPos: e.target.rowPos, evt: e})
    }, _onmousein: function (e) {
        var t;
        return t = this.model.get("id"), e.seqId = t, this.g.trigger("residue:mousein", {seqId: t, rowPos: e.target.rowPos, evt: e})
    }, _onmouseout: function (e) {
        var t;
        return t = this.model.get("id"), e.seqId = t, this.g.trigger("residue:mousein", {seqId: t, rowPos: e.target.rowPos, evt: e})
    }, _drawResidue: function (e, t, n) {
        var i;
        if ("pid" === this.g.colorscheme.get("scheme")) {
            if (i = this.g.columns.get("conserv")[n], i > .8)return e.style.backgroundColor = "#6464ff";
            if (i > .6 && .8 >= i)return e.style.backgroundColor = "#9da5ff";
            if (i > .4 && .6 >= i)return e.style.backgroundColor = "#cccccc"
        } else if (!(this.model.get("grey").indexOf(n) >= 0))return e.className = "biojs-msa-aa-" + t
    }, _getSelection: function (e) {
        var t, n, i, r, o, s, u, l, c, h, d, f, g;
        if (t = e.get("seq").length, o = [], s = this.g.selcol.getSelForRow(e.get("id")), i = a.find(s, function (e) {
            return"row" === e.get("type")
        }), null != i)for (n = u = 0, d = t - 1; d >= u; n = u += 1)o.push(n); else if (s.length > 0)for (l = 0, h = s.length; h > l; l++)for (r = s[l], n = c = f = r.get("xStart"), g = r.get("xEnd"); g >= c; n = c += 1)o.push(n);
        return o
    }, _getPrevNextSelection: function () {
        var e, t, n, i;
        return i = this.model.collection.prev(this.model), n = this.model.collection.next(this.model), null != i && (t = this._getSelection(i)), null != n && (e = this._getSelection(n)), [t, e]
    }, _renderSelection: function (e, t, n, i) {
        var r, s, a, u, l, c, h, d, f;
        for (c = 0, s = d = e, f = this.model.get("seq").length - 1; f >= d && (null != n && n.indexOf(e) >= 0 && (u = !0), null != i && i.indexOf(e) >= 0 && (a = !0), t.indexOf(s) >= 0); s = d += 1)c++;
        return h = this.g.zoomer.get("columnWidth") * c + 1, r = this.g.zoomer.get("rowHeight"), l = o.base({height: 20, width: h}), l.type = "selection", l.style.position = "absolute", l.style.left = 0, l.appendChild(o.line({x1: 0, y1: 0, x2: 0, y2: r, style: "stroke:red;stroke-width:2;"})), l.appendChild(o.line({x1: h, y1: 0, x2: h, y2: r, style: "stroke:red;stroke-width:2;"})), u || l.appendChild(o.line({x1: 0, y1: 0, x2: h, y2: 0, style: "stroke:red;stroke-width:2;"})), a || l.appendChild(o.line({x1: 0, y1: r, x2: h, y2: r, style: "stroke:red;stroke-width:1;"})), l
    }, appendFeature: function (e) {
        var t, n, i;
        return i = 15 * (e.get("xEnd") - e.get("xStart")), n = o.base({height: 20, width: i}), t = e.get("fillColor"), n.appendChild(o.rect({x: 0, y: 0, width: i, height: 5, style: "fill: " + t + ";fill-opacity: 0.4"})), n.style.position = "absolute", r(n).on("mouseover", function (t) {
            return function () {
                return t.g.trigger("feature", e.get("text") + " hovered")
            }
        }(this)), n
    }, removeViews: function () {
        return console.log("seq removed")
    }}), t.exports = n
}, {"../bone/view": 49, "../utils/dom": 78, "../utils/svg": 82, jbone: 42, underscore: 43}], 87: [function (e, t) {
    var n, i, r, o, s, a;
    s = e("../bone/pluginator"), n = e("./AlignmentBody"), i = e("./header/HeaderBlock"), r = e("./OverviewBox"), o = e("../algo/identityCalc"), a = e("underscore"), t.exports = s.extend({initialize: function (e) {
        return this.g = e.g, this.draw(), this.listenTo(this.model, "reset", function () {
            return this.isNotDirty = !1, this.rerender()
        }), this.listenTo(this.model, "change:hidden", a.debounce(this.rerender, 10)), this.listenTo(this.model, "sort", this.rerender), this.listenTo(this.model, "add", function () {
            return console.log("seq add")
        }), this.listenTo(this.g.vis, "change:sequences", this.rerender)
    }, draw: function () {
        var e, t, s, a;
        return this.removeViews(), this.isNotDirty || (t = this.g.consensus.getConsensus(this.model), o(this.model, t), this.isNotDirty = !0), this.g.vis.get("overviewbox") && (a = new r({model: this.model, g: this.g}), a.ordering = -30, this.addView("overviewbox", a)), s = new i({model: this.model, g: this.g}), s.ordering = -1, this.addView("headerblock", s), e = new n({model: this.model, g: this.g}), e.ordering = 0, this.addView("body", e)
    }, render: function () {
        return this.renderSubviews(), this.el.className = "biojs_msa_stage", this
    }, rerender: function () {
        return this.draw(), this.render()
    }})
}, {"../algo/identityCalc": 45, "../bone/pluginator": 48, "./AlignmentBody": 83, "./OverviewBox": 84, "./header/HeaderBlock": 92, underscore: 43}], 88: [function (e, t) {
    t.exports = {A: "ad0052", B: "0c00f3", C: "c2003d", D: "0c00f3", E: "0c00f3", F: "cb0034", G: "6a0095", H: "1500ea", I: "ff0000", J: "fff", K: "0000ff", L: "ea0015", M: "b0004f", N: "0c00f3", O: "fff", P: "4600b9", Q: "0c00f3", R: "0000ff", S: "5e00a1", T: "61009e", U: "fff", V: "f60009", W: "5b00a4", X: "680097", Y: "4f00b0", Z: "0c00f3"}
}, {}], 89: [function (e, t) {
    t.exports = {V: "99ff00", I: "66ff00", L: "33ff00", F: "00ff66", Y: "00ffcc", W: "00ccff", H: "0066ff", R: "0000ff", K: "6600ff", N: "cc00ff", Q: "ff00cc", E: "ff0066", D: "ff0000", S: "ff3300", T: "ff6600", G: "ff9900", P: "ffcc00", C: "ffff00"}
}, {}], 90: [function (e, t) {
    t.exports = {A: "ff6666", B: "fff", C: "ffff00", D: "33cc00", E: "33cc00", F: "ff9900", G: "cc33cc", H: "cc0000", I: "ff6666", J: "fff", K: "cc0000", L: "ff6666", M: "ff6666", N: "3366ff", O: "fff", P: "cc33cc", Q: "3366ff", R: "cc0000", S: "3366ff", T: "3366ff", U: "fff", V: "ff6666", W: "ff9900", X: "fff", Y: "ff9900", Z: "fff"}
}, {}], 91: [function (e, t) {
    var n, i, r, o;
    o = e("../../bone/view"), i = e("../../utils/dom"), r = e("../../utils/svg"), n = o.extend({className: "biojs_msa_conserv", initialize: function (e) {
        return this.g = e.g, this.listenTo(this.g.zoomer, "change:stepSize change:labelWidth change:columnWidth", this.render), this.listenTo(this.g.vis, "change:labels change:metacell", this.render), this.listenTo(this.g.columns, "change:scaling", this.render), this.listenTo(this.model, "reset", this.render), this.manageEvents()
    }, render: function () {
        var e, t, n, o, s, a, u, l, c, h, d, f, g, p, m;
        for (this.g.columns.calcConservation(this.model), i.removeAllChilds(this.el), l = this.model.getMaxLength(), t = this.g.zoomer.get("columnWidth"), a = 20, f = t * (l - this.g.columns.get("hidden").length), h = r.base({height: a, width: f}), h.style.display = "inline-block", h.style.cursor = "pointer", d = this.g.zoomer.get("stepSize"), o = this.g.columns.get("hidden"), g = 0, u = 0; l > u;)if (o.indexOf(u) >= 0)u += d; else {
            for (f = t * d, e = 0, s = p = 0, m = d - 1; m >= 0 ? m >= p : p >= m; s = m >= 0 ? ++p : --p)e += this.g.columns.get("conserv")[u];
            n = a * (e / d), c = r.rect({x: g, y: a - n, width: f - t / 4, height: n, style: "stroke:red;stroke-width:1;"}), c.rowPos = u, h.appendChild(c), g += f, u += d
        }
        return this.el.appendChild(h), this
    }, _onclick: function (e) {
        var t, n, i, r, o, s;
        for (n = e.target.rowPos, i = this.g.zoomer.get("stepSize"), s = [], t = r = 0, o = i - 1; o >= r; t = r += 1)s.push(this.g.trigger("bar:click", {rowPos: n + t, evt: e}));
        return s
    }, manageEvents: function () {
        var e;
        return e = {}, e.click = "_onclick", this.g.config.get("registerMouseEvents") && (e.mousein = "_onmousein", e.mouseout = "_onmouseout"), this.delegateEvents(e), this.listenTo(this.g.config, "change:registerMouseEvents", this.manageEvents)
    }, _onmousein: function (e) {
        var t;
        return t = this.g.zoomer.get("stepSize" * e.rowPos), this.g.trigger("bar:mousein", {rowPos: t, evt: e})
    }, _onmouseout: function (e) {
        var t;
        return t = this.g.zoomer.get("stepSize" * e.rowPos), this.g.trigger("bar:mouseout", {rowPos: t, evt: e})
    }}), t.exports = n
}, {"../../bone/view": 49, "../../utils/dom": 78, "../../utils/svg": 82}], 92: [function (e, t) {
    var n, i, r, o, s;
    i = e("./MarkerView.coffee"), n = e("./ConservationView"), r = e("../../algo/identityCalc"), o = e("../../bone/pluginator"), s = e("underscore"), t.exports = o.extend({initialize: function (e) {
        return this.g = e.g, this.listenTo(this.g.vis, "change:markers change:conserv", function () {
            return this.draw(), this.render()
        }), this.listenTo(this.g.vis, "change", this._setSpacer), this.listenTo(this.g.zoomer, "change:alignmentWidth", function () {
            return this._adjustWidth()
        }), this.listenTo(this.g.columns, "change:hidden", function () {
            return this.draw(), this.render()
        }), this.draw(), this._onscroll = this.model.length <= 10 ? this._sendScrollEvent : s.debounce(this._sendScrollEvent, 500)
    }, events: {scroll: "_onscroll"}, draw: function () {
        var e, t, o;
        return this.removeViews(), this.isNotDirty || (e = this.g.consensus.getConsensus(this.model), r(this.model, e), this.isNotDirty = !0), this.g.vis.get("conserv") && (t = new n({model: this.model, g: this.g}), t.ordering = -20, this.addView("conserv", t)), this.g.vis.get("markers") ? (o = new i({model: this.model, g: this.g}), o.ordering = -10, this.addView("marker", o)) : void 0
    }, render: function () {
        return this.renderSubviews(), this._setSpacer(), this.el.className = "biojs_msa_header", this.el.style.overflowX = "auto", this._adjustWidth(), this
    }, _sendScrollEvent: function () {
        return this.g.zoomer.set("_alignmentScrollLeft", this.el.scrollLeft)
    }, _setSpacer: function () {
        return this.el.style.marginLeft = this._getLabelWidth()
    }, _getLabelWidth: function () {
        var e;
        return e = 0, this.g.vis.get("labels") && (e += this.g.zoomer.get("labelWidth")), this.g.vis.get("metacell") && (e += this.g.zoomer.get("metaWidth")), e
    }, _adjustWidth: function () {
        return this.el.style.width = this.g.zoomer.get("alignmentWidth")
    }})
}, {"../../algo/identityCalc": 45, "../../bone/pluginator": 48, "./ConservationView": 91, "./MarkerView.coffee": 93, underscore: 43}], 93: [function (e, t) {
    var n, i, r, o, s;
    s = e("../../bone/view"), i = e("../../utils/dom"), o = e("../../utils/svg"), r = e("jbone"), n = s.extend({className: "biojs_msa_marker", initialize: function (e) {
        return this.g = e.g, this.listenTo(this.g.zoomer, "change:stepSize change:labelWidth change:columnWidth", this.render), this.listenTo(this.g.vis, "change:labels change:metacell", this.render), this.manageEvents()
    }, render: function () {
        var e, t, n, r, o, s, a;
        for (i.removeAllChilds(this.el), t = document.createElement("span"), r = 0, e = this.g.zoomer.get("columnWidth"), o = this.model.getMaxLength(), a = this.g.zoomer.get("stepSize"), n = this.g.columns.get("hidden"); o > r;)n.indexOf(r) >= 0 ? (this.markerHidden(s, r, a), r += a) : (s = document.createElement("span"), s.style.width = e * a, s.style.display = "inline-block", s.textContent = r + 1, s.rowPos = r, r += a, t.appendChild(s));
        return this.el.appendChild(t), this
    }, markerHidden: function (e, t, n) {
        var i, s, a, u, l, c, h, d, f, g, p;
        for (i = this.g.columns.get("hidden").slice(0), l = Math.max(0, t - n), h = !0, a = g = l; t >= g; a = g += 1)h &= i.indexOf(a) >= 0;
        if (!h) {
            for (c = this.model.getMaxLength(), u = 0, s = -1, t = p = t; c >= p && (s >= 0 || (s = i.indexOf(t)), i.indexOf(t) >= 0); t = p += 1)u++;
            return d = o.base({height: 10, width: 10}), d.style.position = "relative", f = o.polygon({points: "0,0 5,5 10,0", style: "fill:lime;stroke:purple;stroke-width:1"}), r(f).on("click", function (e) {
                return function () {
                    return i.splice(s, u), e.g.columns.set("hidden", i)
                }
            }(this)), d.appendChild(f), e.appendChild(d), d
        }
    }, manageEvents: function () {
        var e;
        return e = {}, e.click = "_onclick", this.g.config.get("registerMouseEvents") && (e.mousein = "_onmousein", e.mouseout = "_onmouseout"), this.delegateEvents(e), this.listenTo(this.g.config, "change:registerMouseEvents", this.manageEvents)
    }, _onclick: function (e) {
        var t, n;
        return t = e.target.rowPos, n = this.g.zoomer.get("stepSize"), this.g.trigger("column:click", {rowPos: t, stepSize: n, evt: e})
    }, _onmousein: function (e) {
        var t, n;
        return t = this.g.zoomer.get("stepSize" * e.rowPos), n = this.g.zoomer.get("stepSize"), this.g.trigger("column:mousein", {rowPos: t, stepSize: n, evt: e})
    }, _onmouseout: function (e) {
        var t, n;
        return t = this.g.zoomer.get("stepSize" * e.rowPos), n = this.g.zoomer.get("stepSize"), this.g.trigger("column:mouseout", {rowPos: t, stepSize: n, evt: e})
    }}), t.exports = n
}, {"../../bone/view": 49, "../../utils/dom": 78, "../../utils/svg": 82, jbone: 42}], 94: [function (e, t) {
    var n, i;
    n = e("./LabelRowView"), i = e("../../bone/pluginator"), t.exports = i.extend({initialize: function (e) {
        return this.g = e.g, this.draw()
    }, draw: function () {
        var e, t, i, r, o;
        for (this.removeViews(), o = [], e = i = 0, r = this.model.length - 1; r >= i; e = i += 1)this.model.at(e).get("hidden") || (t = new n({model: this.model.at(e), g: this.g}), t.ordering = e, o.push(this.addView("row_" + e, t)));
        return o
    }, render: function () {
        return this.renderSubviews(), this.el.className = "biojs_msa_labelblock", this.el.style.display = "inline-block", this.el.style.verticalAlign = "top", this
    }})
}, {"../../bone/pluginator": 48, "./LabelRowView": 95}], 95: [function (e, t) {
    var n, i, r;
    r = e("../../bone/pluginator"), n = e("./LabelView"), i = e("./MetaView"), t.exports = r.extend({initialize: function (e) {
        return this.g = e.g, this.draw(), this.listenTo(this.g.vis, "change:labels", this.drawR), this.listenTo(this.g.vis, "change:metacell", this.drawR)
    }, draw: function () {
        return this.removeViews(), this.g.vis.get("labels") && this.addView("labels", new n({model: this.model, g: this.g})), this.g.vis.get("metacell") ? this.addView("metacell", new i({model: this.model, g: this.g})) : void 0
    }, drawR: function () {
        return this.draw(), this.render()
    }, render: function () {
        return this.renderSubviews(), this.el.setAttribute("class", "biojs_msa_labelrow"), this.el.style.height = this.g.zoomer.get("rowHeight"), this
    }})
}, {"../../bone/pluginator": 48, "./LabelView": 96, "./MetaView": 97}], 96: [function (e, t) {
    var n, i, r;
    r = e("../../bone/view"), i = e("../../utils/dom"), n = r.extend({initialize: function (e) {
        return this.seq = e.seq, this.g = e.g, this.manageEvents()
    }, manageEvents: function () {
        var e;
        return e = {}, e.click = "_onclick", this.g.config.get("registerMouseEvents") && (e.mousein = "_onmousein", e.mouseout = "_onmouseout"), this.delegateEvents(e), this.listenTo(this.g.config, "change:registerMouseEvents", this.manageEvents)
    }, render: function () {
        var e, t;
        return i.removeAllChilds(this.el), this.el.style.width = "" + this.g.zoomer.get("labelWidth") + "px", this.el.style.height = "" + this.g.zoomer.get("rowHeight") + "px", this.el.style.fontSize = "" + this.g.zoomer.get("labelFontsize") + "px", this.el.setAttribute("class", "biojs_msa_labels"), this.g.vis.get("labelId") && (e = document.createElement("span"), e.textContent = this.model.get("id"), e.style.width = 30, e.style.display = "inline-block", this.el.appendChild(e)), this.g.vis.get("labelName") && (t = document.createElement("span"), t.textContent = this.model.get("name").substring(0, 30), this.el.appendChild(t)), this.el.style.overflow = scroll, this
    }, _onclick: function (e) {
        var t;
        return t = this.model.get("id"), this.g.trigger("row:click", {seqId: t, evt: e})
    }, _onmousein: function (e) {
        var t;
        return t = this.model.get("id"), this.g.trigger("row:mouseout", {seqId: t, evt: e})
    }, _onmouseout: function (e) {
        var t;
        return t = this.model.get("id"), this.g.trigger("row:mouseout", {seqId: t, evt: e})
    }}), t.exports = n
}, {"../../bone/view": 49, "../../utils/dom": 78}], 97: [function (e, t) {
    var n, i, r, o, s;
    o = e("../../bone/view"), n = e("../../menu/menubuilder"), s = e("underscore"), r = e("../../utils/dom"), t.exports = i = o.extend({className: "biojs_msa_metaview", initialize: function (e) {
        return this.g = e.g
    }, events: {click: "_onclick", mousein: "_onmousein", mouseout: "_onmouseout"}, render: function () {
        var e, t, i, o, a, u, l;
        return r.removeAllChilds(this.el), this.el.style.display = "inline-block", l = this.g.zoomer.get("metaWidth"), this.el.style.width = l - 5, this.el.style.paddingRight = 5, u = this.model.get("seq"), t = s.reduce(u, function (e, t) {
            return"-" === t && e++, e
        }, 0), t = (t / u.length).toFixed(1), e = document.createElement("span"), e.textContent = t, e.style.display = "inline-block", e.style.width = 35, this.el.appendChild(e), i = this.model.get("identity"), o = document.createElement("span"), o.textContent = i.toFixed(2), o.style.display = "inline-block", o.style.width = 40, this.el.appendChild(o), a = new n("↗"), a.addNode("Uniprot", function () {
            return function () {
                return window.open("http://beta.uniprot.org/uniprot/Q7T2N8")
            }
        }(this)), this.el.appendChild(a.buildDOM()), this.el.width = 10, this.el.style.height = "" + this.g.zoomer.get("rowHeight") + "px", this.el.style.cursor = "pointer"
    }, _onclick: function (e) {
        return this.g.trigger("meta:click", {seqId: this.model.get("id", {evt: e})})
    }, _onmousein: function (e) {
        return this.g.trigger("meta:mousein", {seqId: this.model.get("id", {evt: e})})
    }, _onmouseout: function (e) {
        return this.g.trigger("meta:mouseout", {seqId: this.model.get("id", {evt: e})})
    }})
}, {"../../bone/view": 49, "../../menu/menubuilder": 61, "../../utils/dom": 78, underscore: 43}]}, {}, [2]);