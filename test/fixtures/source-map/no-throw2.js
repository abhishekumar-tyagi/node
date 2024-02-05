var Foo = /** @class */ (function () {
    function Foo(x) {
        if (x === void 0) { x = 33; }
        this.x = x ? x : 99;
        if (this.x) {
            this.methodA();
        }
        else {
            this.methodB();
        }
        this.methodC();
    }
    Foo.prototype.methodA = function () {
    };
    Foo.prototype.methodB = function () {
    };
    Foo.prototype.methodC = function () {
    };
    Foo.prototype.methodD = function () {
    };
    return Foo;
}());
var a = new Foo(0);
var b = new Foo(33);
a.methodD();
module.exports = {
    a: a,
    b: b,
    Foo: Foo,
};
// To recreate:
//
// npx tsc --outDir test/fixtures/source-map --sourceMap --inlineSources test/fixtures/source-map/no-throw.ts
// cp test/fixtures/source-map/no-throw.ts test/fixtures/source-map/no-throw2.ts
//# sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoibm8tdGhyb3cuanMiLCJzb3VyY2VSb290IjoiIiwic291cmNlcyI6WyJuby10aHJvdy50cyJdLCJuYW1lcyI6W10sIm1hcHBpbmdzIjoiQUFBQTtJQUVFLGFBQWEsQ0FBTTtRQUFOLGtCQUFBLEVBQUEsTUFBTTtRQUNqQixJQUFJLENBQUMsQ0FBQyxHQUFHLENBQUMsQ0FBQyxDQUFDLENBQUMsQ0FBQyxDQUFDLENBQUMsQ0FBQyxFQUFFLENBQUE7UUFDbkIsSUFBSSxJQUFJLENBQUMsQ0FBQyxFQUFFLENBQUM7WUFDWCxJQUFJLENBQUMsT0FBTyxFQUFFLENBQUE7UUFDaEIsQ0FBQzthQUFNLENBQUM7WUFDTixJQUFJLENBQUMsT0FBTyxFQUFFLENBQUE7UUFDaEIsQ0FBQztRQUNELElBQUksQ0FBQyxPQUFPLEVBQUUsQ0FBQTtJQUNoQixDQUFDO0lBQ0QscUJBQU8sR0FBUDtJQUVBLENBQUM7SUFDRCxxQkFBTyxHQUFQO0lBRUEsQ0FBQztJQUNELHFCQUFPLEdBQVA7SUFFQSxDQUFDO0lBQ0QscUJBQU8sR0FBUDtJQUVBLENBQUM7SUFDSCxVQUFDO0FBQUQsQ0FBQyxBQXZCRCxJQXVCQztBQUVELElBQU0sQ0FBQyxHQUFHLElBQUksR0FBRyxDQUFDLENBQUMsQ0FBQyxDQUFBO0FBQ3BCLElBQU0sQ0FBQyxHQUFHLElBQUksR0FBRyxDQUFDLEVBQUUsQ0FBQyxDQUFBO0FBQ3JCLENBQUMsQ0FBQyxPQUFPLEVBQUUsQ0FBQTtBQU1YLE1BQU0sQ0FBQyxPQUFPLEdBQUc7SUFDZixDQUFDLEdBQUE7SUFDRCxDQUFDLEdBQUE7SUFDRCxHQUFHLEtBQUE7Q0FDSixDQUFBO0FBRUQsZUFBZTtBQUNmLEVBQUU7QUFDRiw2R0FBNkciLCJzb3VyY2VzQ29udGVudCI6WyJjbGFzcyBGb28ge1xuICB4O1xuICBjb25zdHJ1Y3RvciAoeCA9IDMzKSB7XG4gICAgdGhpcy54ID0geCA/IHggOiA5OVxuICAgIGlmICh0aGlzLngpIHtcbiAgICAgIHRoaXMubWV0aG9kQSgpXG4gICAgfSBlbHNlIHtcbiAgICAgIHRoaXMubWV0aG9kQigpXG4gICAgfVxuICAgIHRoaXMubWV0aG9kQygpXG4gIH1cbiAgbWV0aG9kQSAoKSB7XG5cbiAgfVxuICBtZXRob2RCICgpIHtcblxuICB9XG4gIG1ldGhvZEMgKCkge1xuXG4gIH1cbiAgbWV0aG9kRCAoKSB7XG5cbiAgfVxufVxuXG5jb25zdCBhID0gbmV3IEZvbygwKVxuY29uc3QgYiA9IG5ldyBGb28oMzMpXG5hLm1ldGhvZEQoKVxuXG5kZWNsYXJlIGNvbnN0IG1vZHVsZToge1xuICBleHBvcnRzOiBhbnlcbn1cblxubW9kdWxlLmV4cG9ydHMgPSB7XG4gIGEsXG4gIGIsXG4gIEZvbyxcbn1cblxuLy8gVG8gcmVjcmVhdGU6XG4vL1xuLy8gbnB4IHRzYyAtLW91dERpciB0ZXN0L2ZpeHR1cmVzL3NvdXJjZS1tYXAgLS1zb3VyY2VNYXAgLS1pbmxpbmVTb3VyY2VzIHRlc3QvZml4dHVyZXMvc291cmNlLW1hcC9uby10aHJvdy50c1xuIl19